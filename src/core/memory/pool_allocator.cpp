#include "pool_allocator.h"
#include "../debug/logger.h"
#include "memory.h"


namespace Borealis::Memory
{
	using namespace Borealis::Types;
	using namespace Borealis::Debug;
	
	PoolAllocator::PoolAllocator(uint32 poolElementCount, uint64 elementSize)
		: poolElementCount(poolElementCount), poolElementSize(elementSize), freePoolElements(poolElementCount)
	{ 
		Assert(poolElementCount > 0, "The number of pool elements may not be 0!");
		Assert(elementSize > 0, "The pool element size may not be 0!");

		totalMemorySize = poolElementCount * elementSize;
		usedMemorySize = 0;

		// TODO: Align memory to 16 / 32 / 64 (?) bytes
		// TODO: Replace the byte-sized lookup base for a bitmask buffer which is more memory efficient. 
		//	-> This would mean, that the element count needs to be aligned (64 byte) to fit the bitmasks, though.

		// To quickly find available pool elements, a bitmask is used to represent the pools.
		// [ lookup bitmask | pool element 1 | pool element 2 | pool element 3 | pool element 4 | ... ]

		// Calculate the size of the lookup segment and allocate the complete memory for the allocator.
		p_allocatorBase = reinterpret_cast<uint64Ptr>(malloc(this->poolElementCount + totalMemorySize));
		
		// Initialize and reset the lookup segment.
		p_lookupBase = new (reinterpret_cast<bool*>(p_allocatorBase)) bool[this->poolElementCount];
		memset(p_lookupBase, 0, this->poolElementCount);

		//Set the pool base ptr to start after the lookup segment.
		p_poolBase = p_allocatorBase + this->poolElementCount;
	}

	PoolAllocator::~PoolAllocator()
	{
		Clear();
		free((void*)p_allocatorBase);

		poolElementCount = 0;
		poolElementSize = 0;

		p_allocatorBase = 0;
		p_poolBase = 0;
		p_lookupBase = nullptr;
	}

	void* PoolAllocator::RawAlloc(const uint16 allocSize)
	{
		Assert(allocSize <= poolElementSize, "The size of an allocated object (%u) may not exceed the pool element size (%u)", allocSize, poolElementSize);

		if (freePoolElements <= 0)
		{
			LogError("Pool ran out of free elements!");
			return nullptr;
		}

		void* p_freePoolElement = nullptr;

		// If allocation is possible, lookup the next free pool element and return its base ptr.
		for (int32 poolElementIndex = 0; poolElementIndex < poolElementCount; ++poolElementIndex)
		{
			if (p_lookupBase[poolElementIndex] == false)
			{
				p_lookupBase[poolElementIndex] = true;
				p_freePoolElement = reinterpret_cast<void*>(p_poolBase + (poolElementSize * poolElementIndex));
				break;
			}
		}

		// Update internal data
		--freePoolElements;
		usedMemorySize += poolElementSize;

		++allocationCount;

		return p_freePoolElement;
	}

	HandleInfo* PoolAllocator::Alloc(const uint16 allocSize)
	{
		Assert(allocSize <= poolElementSize,
			"The size of an allocated object (%u) may not exceed the pool element size (%u)", allocSize, poolElementSize);

		if (freePoolElements <= 0)
		{
			LogError("Pool ran out of free elements!");
			return nullptr;
		}

		void* p_freePoolElement = nullptr;

		// If allocation is possible, lookup the next free pool element and return its base ptr.
		for (int32 poolElementIndex = 0; poolElementIndex < poolElementCount; ++poolElementIndex)
		{
			if (p_lookupBase[poolElementIndex] == false)
			{
				p_lookupBase[poolElementIndex] = true;
				p_freePoolElement = reinterpret_cast<void*>(p_poolBase + (poolElementSize * poolElementIndex));
				break;
			}
		}

		// Update internal data
		--freePoolElements;
		usedMemorySize += poolElementSize;
		
		++allocationCount;

		return RegisterHandle(p_freePoolElement);
	}

	void PoolAllocator::FreeMemory(const void* const address)
	{
		Assert(reinterpret_cast<uint64Ptr>(address) >= p_poolBase &&
			reinterpret_cast<uint64Ptr>(address) <= p_poolBase + (poolElementCount * poolElementSize) + poolElementSize,
			"The given memory address '0x%p' is not part of the allocator pool!", reinterpret_cast<const void*>(address));

		int32 poolIndex = (reinterpret_cast<uint64Ptr>(address) - p_poolBase) / poolElementSize;
		Assert(poolIndex >= 0 && poolIndex < poolElementCount,
			"Pool index invalid! The index %i is not within the range [0, %i]!", poolIndex, poolElementCount - 1);

		Assert(p_lookupBase[poolIndex],
			"Pool index (%i) points to free pool element. Cannot free an already free pool element!", poolIndex);

		// Check if pool element is actually used.
		if (poolIndex < poolElementCount && p_lookupBase[poolIndex])
		{
#ifdef CLEAR_POOL_ELEMENTS_ON_FREE
			memset(reinterpret_cast<void*>(p_poolBase + (poolElementSize * poolIndex)), 0, poolElementSize);
#endif
			
			usedMemorySize -= poolElementSize;
			++freePoolElements;

			p_lookupBase[poolIndex] = false;
			Assert(allocationCount > 0, "Trying to free memory that has not been allocated using this allocator!");
			++freeCount;
		}
	}

	HandleInfo* PoolAllocator::AllocAligned(const uint16 allocSize)
	{
		Assert(allocSize * 2 <= poolElementSize,
			"The size of an aligned allocated object (%u) may not exceed the pool element size (%u)", allocSize, poolElementSize);

		if (freePoolElements <= 0)
		{
			LogError("Pool ran out of free elements!");
			return nullptr;
		}

		void* p_freePoolElement = nullptr;

		// If allocation is possible, lookup the next free pool element and return its base ptr.
		for (int32 poolElementIndex = 0; poolElementIndex < poolElementCount; ++poolElementIndex)
		{
			if (p_lookupBase[poolElementIndex] == false)
			{
				p_lookupBase[poolElementIndex] = true;

				// Calculate base pointer
				p_freePoolElement = reinterpret_cast<void*>(p_poolBase + (poolElementSize * poolElementIndex));
				
				// Calculate align offset
				uint64Ptr alignOffset = (allocSize - (reinterpret_cast<uint64Ptr>(p_freePoolElement) % allocSize));

				// Offset base pointer for proper alignment
				p_freePoolElement = 
					reinterpret_cast<void*>(reinterpret_cast<uint64Ptr>(p_freePoolElement) + alignOffset);
				
				// Store align offset right before the data
				uint64Ptr p_alignOffsetAddress = reinterpret_cast<uint64Ptr>(p_freePoolElement) - sizeof(AllocationOffset);
				AllocationOffset* p_alignOffset = new (reinterpret_cast<void*>(p_alignOffsetAddress)) AllocationOffset();

				*p_alignOffset = alignOffset;
				break;
			}
		}

		// Update internal data
		--freePoolElements;
		usedMemorySize += poolElementSize;

		++allocationCount;
		
		return RegisterHandle(p_freePoolElement);
	}

	void PoolAllocator::FreeAligned(const void* const address)
	{
		Assert(reinterpret_cast<uint64Ptr>(address) >= p_poolBase &&
			reinterpret_cast<uint64Ptr>(address) <= p_poolBase + (poolElementCount * poolElementSize) + poolElementSize,
			"The given memory address '0x%p' is not part of the allocator pool!", reinterpret_cast<const void*>(address));

		uint64Ptr poolBaseAddress = reinterpret_cast<uint64Ptr>(address) - sizeof(AllocationOffset);
		AllocationOffset* p_alignOffset = reinterpret_cast<AllocationOffset*>(poolBaseAddress);

		uint64Ptr poolAddress = reinterpret_cast<uint64Ptr>(address) - *p_alignOffset;

		int32 poolIndex = (poolAddress - p_poolBase) / poolElementSize;
		Assert(poolIndex >= 0 && poolIndex < poolElementCount, 
			"Pool index invalid! The index %i is not within the range [0, %i]!", poolIndex, poolElementCount - 1);

		Assert(p_lookupBase[poolIndex], "Pool index (%i) points to free pool element. Cannot free an already free pool element!", poolIndex);

		// Check if pool element is actually used.
		if (poolIndex < poolElementCount && p_lookupBase[poolIndex])
		{
#ifdef CLEAR_POOL_ELEMENTS_ON_FREE 
			memset(reinterpret_cast<void*>(p_poolBase + (poolElementSize * poolIndex)), 0, poolElementSize);
#endif

			usedMemorySize -= poolElementSize;
			++freePoolElements;

			p_lookupBase[poolIndex] = false;
			Assert(allocationCount > 0, "Trying to free memory that has not been allocated using this allocator!");
			++freeCount;
		}
	}

	void PoolAllocator::Clear()
	{
		memset(p_lookupBase, 0, poolElementCount);

		freePoolElements = poolElementCount;
		usedMemorySize = 0;
		freeCount = 0;
		allocationCount = 0;
	}

	uint64 PoolAllocator::GetTotalMemorySize() const
	{
		return totalMemorySize;
	}
	
	uint64 PoolAllocator::GetUsedMemorySize() const
	{
		return usedMemorySize;
	}
	
	uint64 PoolAllocator::GetAvailableMemorySize() const
	{
		return totalMemorySize - usedMemorySize;
	}
	
	int8 PoolAllocator::GetAllocFreeRatio() const
	{
		return static_cast<int8>(allocationCount - freeCount);
	}
}