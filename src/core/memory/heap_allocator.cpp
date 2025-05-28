#include "heap_allocator.h"
#include "ref_cnt_auto_ptr.h"

using namespace Borealis::Types;
namespace Borealis::Memory
{
	HeapAllocator::HeapAllocator()
	{
		IMemoryAllocator::totalMemorySize = 4096;
		IMemoryAllocator::usedMemorySize = 0;
		
		// Initialize heap memory
		heapBasePtr = malloc(IMemoryAllocator::totalMemorySize);
		p_freeMemBlockList = new std::list<HeapFreeListEntry>();

		// Insert initial free memory block
		p_freeMemBlockList->emplace_back(
			HeapFreeListEntry(
				heapBasePtr, (reinterpret_cast<void*>(reinterpret_cast<uint64Ptr>(heapBasePtr) + totalMemorySize))
			)
		);
	}
	
	HeapAllocator::HeapAllocator(uint64 size)
	{
		IMemoryAllocator::totalMemorySize = size;
		IMemoryAllocator::usedMemorySize = 0;
		
		heapBasePtr = malloc(IMemoryAllocator::totalMemorySize);
		p_freeMemBlockList = new std::list<HeapFreeListEntry>();

		// Insert initial free memory block
		p_freeMemBlockList->emplace_back(
			HeapFreeListEntry(
				heapBasePtr, (reinterpret_cast<void*>(reinterpret_cast<uint64Ptr>(heapBasePtr) + totalMemorySize))
			)
		);
	}

	HeapAllocator::HeapAllocator(HeapAllocator&& other) noexcept
		: heapBasePtr(other.heapBasePtr), p_freeMemBlockList(other.p_freeMemBlockList)
	{
		IMemoryAllocator::allocationCount = other.allocationCount;
		IMemoryAllocator::freeCount = other.freeCount;
		IMemoryAllocator::totalMemorySize = other.totalMemorySize;
		IMemoryAllocator::usedMemorySize = other.usedMemorySize;

		other.heapBasePtr = 0;
		other.p_freeMemBlockList = nullptr;
		other.allocationCount = 0;
		other.freeCount = 0;
		other.totalMemorySize = 0;
		other.usedMemorySize = 0;
	}
	
	HeapAllocator::~HeapAllocator()
	{
		free(heapBasePtr);
		delete p_freeMemBlockList;
	}

	HeapAllocator& HeapAllocator::operator=(HeapAllocator&& other) noexcept
	{
		heapBasePtr = other.heapBasePtr;
		p_freeMemBlockList = other.p_freeMemBlockList;

		IMemoryAllocator::allocationCount = other.allocationCount;
		IMemoryAllocator::freeCount = other.freeCount;
		IMemoryAllocator::totalMemorySize = other.totalMemorySize;
		IMemoryAllocator::usedMemorySize = other.usedMemorySize;

		other.heapBasePtr = 0;
		other.p_freeMemBlockList = nullptr;
		other.allocationCount = 0;
		other.freeCount = 0;
		other.totalMemorySize = 0;
		other.usedMemorySize = 0;

		return *this;
	}

	HandleInfo* HeapAllocator::Alloc(const uint16 allocSize)
	{
		Debug::Assert(p_freeMemBlockList != nullptr && p_freeMemBlockList->size() > 0, 
			"The heap allocator does not seem to be inititalized yet!");
		Debug::Assert(totalMemorySize > (allocSize + sizeof(HeapDescription)),
			"The requested allocation size exceeds the total memory available in the heap allocator!");

		for (std::list<HeapFreeListEntry>::iterator it = p_freeMemBlockList->begin(); it != p_freeMemBlockList->end(); ++it)
		{
			if (it->AvailableSize() > allocSize + sizeof(HeapDescription))
			{
				++allocationCount;
				usedMemorySize += allocSize + sizeof(HeapDescription);
				
				return it->AllocMemoryFromBlock(allocSize);
			}
		}
		
		// Failed to obtain a valid memory block from the heap!
		return nullptr;
	}
	
	HandleInfo* HeapAllocator::AllocAligned(const uint16 allocSize)
	{
		Debug::Assert(p_freeMemBlockList != nullptr && p_freeMemBlockList->size() > 0, 
			"The heap allocator does not seem to be inititalized yet!");
		Debug::Assert(totalMemorySize > (allocSize + sizeof(HeapDescription)),
			"The requested allocation size exceeds the total memory available in the heap allocator!");

		for (std::list<HeapFreeListEntry>::iterator it = p_freeMemBlockList->begin(); it != p_freeMemBlockList->end(); ++it)
		{
			if (it->AvailableSize() > allocSize * 2)
			{
				++allocationCount;
				usedMemorySize += allocSize + sizeof(HeapDescription);

				return it->AllocMemoryFromBlock(allocSize, true);
			}
		}

		// Failed to obtain a valid memory block from the heap!
		return nullptr;
	}

	bool HeapAllocator::ReturnFreeBlock(std::list<HeapFreeListEntry>::iterator nextFreeMemBlock, const HeapDescription* const p_returnedMemBlockDesc)
	{
		Debug::Assert(p_returnedMemBlockDesc->Size > 0 && p_returnedMemBlockDesc->HandleId > 0, 
			"Invalid heap description! Make sure, the heap description is valid and not erased before returning the mem block.");

		bool merged = false;
		
		// Merge with previous memblock
		if (nextFreeMemBlock != p_freeMemBlockList->begin())
		{
			std::list<HeapFreeListEntry>::iterator previousBlockIterator = nextFreeMemBlock;
			--previousBlockIterator;
			
			// This next line is only for debugging purposes and can be safely deleted!
			void* p_memBlockStart = reinterpret_cast<void*>(p_returnedMemBlockDesc->GetMemBlockStart());
			
			if (p_returnedMemBlockDesc->GetMemBlockStart() == reinterpret_cast<uint64Ptr>(previousBlockIterator->p_BlockEnd))
			{
				// Can merge, modify free list entry and set new block end
				previousBlockIterator->p_BlockEnd = reinterpret_cast<void*>(p_returnedMemBlockDesc->GetMemBlockEnd());

				++freeCount;
				usedMemorySize -= p_returnedMemBlockDesc->Size + sizeof(HeapDescription);

#ifdef CLEAR_HEAP_ELEMENT_ON_FREE
				memset(reinterpret_cast<void*>(p_returnedMemBlockDesc->GetMemBlockStart()), 0,
					sizeof(HeapDescription) + p_returnedMemBlockDesc->AlignOffset + p_returnedMemBlockDesc->Size);
#endif

				merged = true;
			}
		}

		if(!merged)	// If not already merged, try to merge with next free block
		{
			// Merge with next memblock (current memblock is this memblock!)
			std::list<HeapFreeListEntry>::iterator currentBlockIterator = nextFreeMemBlock;

			if (currentBlockIterator != p_freeMemBlockList->end())
			{
				if (p_returnedMemBlockDesc->GetMemBlockEnd() == reinterpret_cast<uint64Ptr>(currentBlockIterator->p_BlockStart))
				{
					currentBlockIterator->p_BlockStart = reinterpret_cast<void*>(p_returnedMemBlockDesc->GetMemBlockStart());

					++freeCount;
					usedMemorySize -= p_returnedMemBlockDesc->Size + sizeof(HeapDescription);

#ifdef CLEAR_HEAP_ELEMENT_ON_FREE
					memset(reinterpret_cast<void*>(p_returnedMemBlockDesc->GetMemBlockStart()), 0,
						sizeof(HeapDescription) + p_returnedMemBlockDesc->AlignOffset + p_returnedMemBlockDesc->Size);
#endif

					merged = true;
				}
			}
		}
		else // If previous and returned mem block have been merged, try to merge with next free memblock if possible
		{
			// This now includes the returned memory block
			std::list<HeapFreeListEntry>::iterator previousBlockIterator = nextFreeMemBlock;
			--previousBlockIterator;

			if (nextFreeMemBlock != p_freeMemBlockList->end() && 
				HeapFreeListEntry::IsAdjacent(*previousBlockIterator, *nextFreeMemBlock))
			{
				MergeFreeBlocks(previousBlockIterator, nextFreeMemBlock);
			}
		}
		
		return merged;
	}

	void HeapAllocator::FreeMemory(const void* const address)
	{
		Debug::Assert(p_freeMemBlockList != nullptr && p_freeMemBlockList->size() > 0,
			"The heap allocator does not seem to be inititalized yet!");
		{
			std::list<HeapFreeListEntry>::iterator nextFreeMemBlock;

			for (nextFreeMemBlock = p_freeMemBlockList->begin(); 
				nextFreeMemBlock != p_freeMemBlockList->end(); ++nextFreeMemBlock)
			{
				if (nextFreeMemBlock->p_BlockStart > address)
				{
					// Find heap desc by reading pre data align offset!
					const uint16* p_preDataAlignOffset = 
						reinterpret_cast<uint16*>(reinterpret_cast<uint64Ptr>(address) - sizeof(uint16));
					const HeapDescription* const p_returnedMemBlockDesc = 
						reinterpret_cast<HeapDescription*>(reinterpret_cast<uint64Ptr>(address) - (*p_preDataAlignOffset) - sizeof(HeapDescription));
					
					Debug::Assert(p_returnedMemBlockDesc != nullptr && p_returnedMemBlockDesc->Size > 0, 
						"Couldn't read heap description!");
					
					const uint64Ptr memBlockBase = reinterpret_cast<uint64Ptr>(address) - p_returnedMemBlockDesc->AlignOffset - sizeof(HeapDescription);
					
					if (ReturnFreeBlock(nextFreeMemBlock, p_returnedMemBlockDesc))
						return;

					// Cannot merge, insert instead!
					p_freeMemBlockList->insert(nextFreeMemBlock,
						HeapFreeListEntry(reinterpret_cast<void*>(memBlockBase),
							reinterpret_cast<void*>(reinterpret_cast<uint64Ptr>(address) + p_returnedMemBlockDesc->Size)));

					++freeCount;
					usedMemorySize -= p_returnedMemBlockDesc->Size + sizeof(HeapDescription);
					return;
				}
			}
		}
	}

	void HeapAllocator::FreeAligned(const void* const address)
	{
		// The heap allocator always frees with respect to alignment.
		// No special implementation is required! See FreeMemory(const void* address) above!
		return FreeMemory(address);
	}

	void HeapAllocator::Clear()
	{
		memset(reinterpret_cast<void*>(heapBasePtr), 0, totalMemorySize);

		p_freeMemBlockList->clear();
		p_freeMemBlockList->emplace_back(HeapFreeListEntry(
			reinterpret_cast<void*>(heapBasePtr), (reinterpret_cast<void*>(reinterpret_cast<uint64Ptr>(heapBasePtr) + totalMemorySize)))
		);

		usedMemorySize = 0;
		freeCount = 0;
		allocationCount = 0;
	}

	void HeapAllocator::Defragment(const Types::uint8 iterations)
	{
		// What if there is one big hole in the middle? -> Actually no defragmentation, but still move?
		if (p_freeMemBlockList->size() < 2) 
			return;

		uint8 currentIteration = 0;

		do
		{
			// Create iterators and validate them
			const std::list<HeapFreeListEntry>::iterator first = p_freeMemBlockList->begin();
			if (first == p_freeMemBlockList->end() || p_freeMemBlockList->size() < 2)
				return;

			std::list<HeapFreeListEntry>::iterator second = first;
			++second;

			// Check for holes between free blocks of memory and move free blocks to higher mem addresses
			{
				if (!HeapFreeListEntry::IsAdjacent(*first, *second))
				{
					// Found a hole!
				
					HeapDescription* p_HeapDesc = reinterpret_cast<HeapDescription*>(reinterpret_cast<uint64Ptr>(first->p_BlockEnd));
					
					Debug::Assert(p_HeapDesc != nullptr && p_HeapDesc->Data() != nullptr && p_HeapDesc->Size > 0,
						"Couldn't find heap description while defragmenting the heap memory!");
					
					const HeapDescription memBlockHeapDescCpy = *p_HeapDesc;
					void* const p_oldMemBlockEnd = reinterpret_cast<void*>(memBlockHeapDescCpy.GetMemBlockEnd());
					
					// Calculate the unaligned new pointer and calculate the new alignment offset
					const uint64Ptr unalignedMovedMemBlockPtr = reinterpret_cast<uint64Ptr>(first->p_BlockStart) + sizeof(HeapDescription);
					const uint16 newAlignmentOffset = unalignedMovedMemBlockPtr % p_HeapDesc->Size == 0 ? 0 : (p_HeapDesc->Size - (unalignedMovedMemBlockPtr % p_HeapDesc->Size));
					void* const p_targetMemBlock = reinterpret_cast<void*>(unalignedMovedMemBlockPtr + newAlignmentOffset);
					
					// Move the memory block & insert the updated HeapDescription + preDataAlignOffset
					memmove(p_targetMemBlock, memBlockHeapDescCpy.Data(), memBlockHeapDescCpy.Size);
					const HeapDescription* p_newHeapDesc = new (first->p_BlockStart) HeapDescription(memBlockHeapDescCpy.HandleId, memBlockHeapDescCpy.Size, newAlignmentOffset);
					uint16* const p_preDataAlignOffset = new (reinterpret_cast<void*>(unalignedMovedMemBlockPtr + newAlignmentOffset - sizeof(uint16))) uint16(newAlignmentOffset);
					
					// Update heap decs value and handle
					*p_preDataAlignOffset = newAlignmentOffset;
					UpdateHandle(p_newHeapDesc->HandleId, p_targetMemBlock);

					// Update the free block description
					first->p_BlockStart = reinterpret_cast<void*>(p_newHeapDesc->GetMemBlockEnd());
					first->p_BlockEnd = p_oldMemBlockEnd;

					Debug::Assert(first->p_BlockStart != first->p_BlockEnd, 
						"Start and end pointers of free block may not point to the same address!");

					if (HeapFreeListEntry::IsAdjacent(*first, *second))
					{
						MergeFreeBlocks(first, second);
					}
				}
			}
		}
		while(++currentIteration < iterations);
	}

	uint64 HeapAllocator::GetTotalMemorySize() const
	{
		return IMemoryAllocator::totalMemorySize;
	}

	uint64 HeapAllocator::GetUsedMemorySize() const
	{
		return IMemoryAllocator::usedMemorySize;
	}

	uint64 HeapAllocator::GetAvailableMemorySize() const
	{
		return IMemoryAllocator::totalMemorySize - IMemoryAllocator::usedMemorySize;
	}

	int8 HeapAllocator::GetAllocFreeRatio() const
	{
		return static_cast<int8>(IMemoryAllocator::allocationCount - IMemoryAllocator::freeCount);
	}

	void HeapAllocator::MergeFreeBlocks(std::list<HeapFreeListEntry>::iterator const first, std::list<HeapFreeListEntry>::iterator const second)
	{
		Debug::Assert(HeapFreeListEntry::IsAdjacent(*first, *second),
			"Trying to merge two non-continuous free memory blocks!");

		second->p_BlockStart = first->p_BlockStart;
		p_freeMemBlockList->erase(first);
	}
	
	HandleInfo* HeapFreeListEntry::AllocMemoryFromBlock(const uint16 blockSize, const bool alignToSize)
	{
		Debug::Assert(alignToSize ? (blockSize * 2) + sizeof(HeapDescription) <= AvailableSize() :
			blockSize + sizeof(HeapDescription) < AvailableSize(),
			"Cannot split a memory block that is smaller than the requested memory block size");

		// Find alignment offset if necessary
		const uint64Ptr p_blockBase = reinterpret_cast<uint64Ptr>(p_BlockStart);
		const uint64Ptr alignOffset = alignToSize ?
			blockSize - ((p_blockBase + sizeof(HeapDescription)) % blockSize) : 0;

		Debug::Assert(alignOffset < 65.536, 
			"Alignment offset is larger than two bytes. Cannot store this offset in front of the data!");
		
		// Calculate final data ptr
		void* p_data = reinterpret_cast<void*>(p_blockBase + sizeof(HeapDescription) + alignOffset);

		HandleInfo* p_hndlInfo = RegisterHandle(p_data);

		// Store and initialize the description at the beginning of the block
		const HeapDescription* p_desc = 
			new (reinterpret_cast<void*>(p_blockBase)) HeapDescription(p_hndlInfo->HandleId, blockSize, alignOffset);

		// Also store align offset pre data pointer! May intersect HeapDescription's padding!
		const uint16* p_preDataAlignOffset = 
			new (reinterpret_cast<void*>(reinterpret_cast<Types::uint64Ptr>(p_data) - sizeof(uint16))) uint16(static_cast<uint16>(alignOffset));

		// Update start of this free list element
		p_BlockStart = reinterpret_cast<void*>(
			reinterpret_cast<Types::uint64Ptr>(p_data) + blockSize);

		return p_hndlInfo;
	}
}