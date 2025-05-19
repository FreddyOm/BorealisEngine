#include "heap_allocator.h"

using namespace Borealis::Types;
namespace Borealis::Memory
{
	HeapAllocator::HeapAllocator()
	{
		IMemoryAllocator::totalMemorySize = 4096;
		IMemoryAllocator::usedMemorySize = 0;
		
		// Initialize heap memory
		heapBasePtr = reinterpret_cast<uint64Ptr>(malloc(IMemoryAllocator::totalMemorySize));
		p_freeMemBlockList = new std::list<HeapFreeListEntry>();

		// Insert initial free memory block
		p_freeMemBlockList->emplace_back(
			HeapFreeListEntry(
				reinterpret_cast<void*>(heapBasePtr), (reinterpret_cast<void*>(heapBasePtr + totalMemorySize))
			)
		);
	}
	
	HeapAllocator::HeapAllocator(uint64 size)
	{
		IMemoryAllocator::totalMemorySize = size;
		IMemoryAllocator::usedMemorySize = 0;
		
		heapBasePtr = reinterpret_cast<uint64Ptr>(malloc(IMemoryAllocator::totalMemorySize));
		p_freeMemBlockList = new std::list<HeapFreeListEntry>();

		// Insert initial free memory block
		p_freeMemBlockList->emplace_back(
			HeapFreeListEntry(
				reinterpret_cast<void*>(heapBasePtr), (reinterpret_cast<void*>(heapBasePtr + totalMemorySize))
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
		free(reinterpret_cast<void*>(heapBasePtr));
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
	
	void* HeapAllocator::Alloc(const uint16 allocSize)
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
	
	void* HeapAllocator::AllocAligned(const uint16 allocSize)
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

	bool HeapAllocator::ReturnFreeBlock(std::list<HeapFreeListEntry>::iterator it, const HeapDescription* const p_returnedMemBlockDesc)
	{
		Debug::Assert(p_returnedMemBlockDesc->Size > 0, "Invalid heap description!");
		bool merged = false;

		// Merge with previous memblock
		if (it != p_freeMemBlockList->begin())
		{
			std::list<HeapFreeListEntry>::iterator previousBlockIterator = it;
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

		// Merge with next memblock (current memblock is this memblock!)
		std::list<HeapFreeListEntry>::iterator currentBlockIterator = it;

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
		
		return merged;
	}

	void HeapAllocator::FreeMemory(const void* address)
	{
		Debug::Assert(p_freeMemBlockList != nullptr && p_freeMemBlockList->size() > 0,
			"The heap allocator does not seem to be inititalized yet!");
		{
			std::list<HeapFreeListEntry>::iterator it;

			for (it = p_freeMemBlockList->begin(); it != p_freeMemBlockList->end(); ++it)
			{
				if (it->p_BlockStart > address)
				{
					const HeapDescription* const p_returnedMemBlockDesc =
						reinterpret_cast<HeapDescription*>(reinterpret_cast<uint64Ptr>(address) - sizeof(HeapDescription));

					Debug::Assert(p_returnedMemBlockDesc != nullptr, "Couldn't read heap description!");

					const uint64Ptr memBlockBase = reinterpret_cast<uint64Ptr>(address) - p_returnedMemBlockDesc->AlignOffset - sizeof(HeapDescription);
					
					if (ReturnFreeBlock(it, p_returnedMemBlockDesc))
						return;

					// Cannot merge, insert instead!
					p_freeMemBlockList->insert(it,
						HeapFreeListEntry(reinterpret_cast<void*>(memBlockBase),
							reinterpret_cast<void*>(reinterpret_cast<uint64Ptr>(address) + p_returnedMemBlockDesc->Size)));

					++freeCount;
					usedMemorySize -= p_returnedMemBlockDesc->Size + sizeof(HeapDescription);
					return;
				}
			}
		}
	}

	void HeapAllocator::FreeAligned(const void* address)
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
			reinterpret_cast<void*>(heapBasePtr), (reinterpret_cast<void*>(heapBasePtr + totalMemorySize)))
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

			// Check for holes between free blocks of memory
			{
				if (reinterpret_cast<uint64Ptr>(first->p_BlockEnd) != reinterpret_cast<uint64Ptr>(second->p_BlockStart))
				{
					// Found a hole!
					
					// @TODO: Look at the beginning of a given used mem block (freeBlock->p_BlockEnd + 1)
					// and read heap description to get data pointer, size and alignment offset

					// Don't forget to realign data, and update HeapDescription accordingly
					// --> To really use this, I need to use handles or always return smart pointers!
				
					HeapDescription* p_HeapDesc = reinterpret_cast<HeapDescription*>(reinterpret_cast<uint64Ptr>(first->p_BlockEnd) + 1);
					
					Debug::Assert(p_HeapDesc != nullptr, 
						"Couldn't find heap description while defragmenting the heap memory!");
					
					HeapDescription memBlockHeapCpy = *p_HeapDesc;
					
					// Calculate the unaligned new pointer and calculate the new alignment offset
					const uint64Ptr movedMemBlockPtr = reinterpret_cast<uint64Ptr>(first->p_BlockStart) + sizeof(HeapDescription);
					const int32 newAlignmentOffset = movedMemBlockPtr % p_HeapDesc->Size == 0 ? 0 : (p_HeapDesc->Size - (movedMemBlockPtr % p_HeapDesc->Size));

					// Move the memory block and insert the updated HeapDescription
					memmove(reinterpret_cast<void*>(movedMemBlockPtr), memBlockHeapCpy.p_Data, memBlockHeapCpy.Size);
					const HeapDescription* p_newHeapDesc = new (first->p_BlockStart) HeapDescription(memBlockHeapCpy.p_Data, memBlockHeapCpy.Size, newAlignmentOffset);

					// Update the free block description
					first->p_BlockStart = reinterpret_cast<void*>(p_newHeapDesc->GetMemBlockEnd() + 1);
					first->p_BlockEnd = reinterpret_cast<void*>(memBlockHeapCpy.GetMemBlockEnd());

					if (reinterpret_cast<uint64Ptr>(first->p_BlockStart) + 1 == reinterpret_cast<uint64Ptr>(first->p_BlockEnd))
					{
						MergeFreeBlocks(first, second);
						p_freeMemBlockList->erase(first);
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
		Debug::Assert(reinterpret_cast<uint64Ptr>(first->p_BlockEnd) + 1 == reinterpret_cast<uint64Ptr>(second->p_BlockStart),
			"Trying to merge two non-continuous free memory blocks!");

		second->p_BlockStart = first->p_BlockStart;
	}
	
	void* HeapFreeListEntry::AllocMemoryFromBlock(const Types::int32 blockSize, const bool alignToSize)
	{
		Debug::Assert(alignToSize ? (blockSize * 2) + sizeof(HeapDescription) <= AvailableSize() :
			blockSize + sizeof(HeapDescription) < AvailableSize(),
			"Cannot split a memory block that is smaller than the requested memory block size");

		const Types::uint64Ptr p_blockBase = reinterpret_cast<Types::uint64Ptr>(p_BlockStart);

		// Find alignment offset if necessary
		const Types::uint64Ptr alignOffset = alignToSize ?
			blockSize - ((p_blockBase + sizeof(HeapDescription)) % blockSize) : 0;

		// Calculate final data ptr and initialize heap description
		uint64Ptr dataPtrAddress = p_blockBase + sizeof(HeapDescription) + alignOffset;
		void* p_data = reinterpret_cast<void*>(dataPtrAddress);

		HeapDescription* p_desc = 
			new (reinterpret_cast<void*>(dataPtrAddress - sizeof(HeapDescription))) HeapDescription(p_data, blockSize, alignOffset);

		// Update start of this free list element
		p_BlockStart = reinterpret_cast<void*>(
			reinterpret_cast<Types::uint64Ptr>(p_data) + blockSize);

		return p_data;
	}
}