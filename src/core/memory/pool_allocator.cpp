#include "pool_allocator.h"
#include "../debug/logger.h"
#include "memory.h"

using namespace Borealis::Types;
using namespace Borealis::Debug;

namespace Borealis::Memory
{
	
	PoolAllocator::PoolAllocator(int32 poolElementCount, uint64 elementSize)
		: m_poolElementCount(poolElementCount), m_poolElementSize(elementSize)
	{ 
		Assert(poolElementCount > 0, "The number of pool elements may not be 0!");
		Assert(elementSize > 0, "The pool element size may not be 0!");

		totalMemorySize = poolElementCount * elementSize;
		usedMemorySize = 0;

		// TODO: Align memory to 16 / 32 / 64 (?) bytes
		// TODO: Replace the byte-sized lookup base for a bitmask buffer which is more memory efficient. 
		//	-> This would mean, that the element count needs to be aligned (64 byte) to fit the bitmasks, though.

		// To quickly find available pool elements, a bitmask is used to represent the pools.
		// [ pool element 0 | pool element 1 | pool element 2 | pool element 3 | pool element 4 | ... ]

		// Calculate the size of the lookup segment and allocate the complete memory for the allocator.
		m_pPoolBase = reinterpret_cast<uint64Ptr>(malloc(totalMemorySize));
		
		for(int32 elementIndex = 0; elementIndex < poolElementCount; ++elementIndex)
		{
			m_pFreePoolElementList.emplace(m_pPoolBase + (elementIndex * elementSize));
		}
		
		// Initialize and reset the lookup segment.
		// p_lookupBase = new (reinterpret_cast<bool*>(p_allocatorBase)) bool[this->poolElementCount];
		// memset(p_lookupBase, 0, this->poolElementCount);

		// //Set the pool base ptr to start after the lookup segment.
		// p_poolBase = p_allocatorBase + this->poolElementCount;
	}

	PoolAllocator::~PoolAllocator()
	{
		Clear();
		free((void*)m_pPoolBase);

		m_poolElementCount = 0;
		m_poolElementSize = 0;

		m_pPoolBase = 0;

		// p_poolBase = 0;
		// p_lookupBase = nullptr;
	}

	/// <summary>
	/// Returns a raw void* and DOES NOT register the allocation inside the debugger!
	/// Try to not use this at all! When allocating with RawAlloc(), the Free() has to be called using the same MemAllocContext as it was allocated with!
	/// </summary>
	/// <param name="allocSize">The size of the desired allocation.</param>
	/// <returns>A raw void* without any handling of alignment, defragmentation or automatic free!</returns>
	void* PoolAllocator::RawAlloc(const uint16 allocSize)
	{
		Assert(allocSize <= m_poolElementSize, "The size of an allocated object (%u) may not exceed the pool element size (%u)", allocSize, m_poolElementSize);

		if (m_pFreePoolElementList.size() <= 0)
		{
			LogError("Pool ran out of free elements!");
			return nullptr;
		}

		void* const p_freePoolElement = GetFreePoolElement();

		// Update internal data
		usedMemorySize += m_poolElementSize;

		++allocationCount;

		return p_freePoolElement;
	}

	HandleInfo* PoolAllocator::Alloc(const uint16 allocSize)
	{
		Assert(allocSize <= m_poolElementSize,
			"The size of an allocated object (%u) may not exceed the pool element size (%u)", allocSize, m_poolElementSize);

		if (m_pFreePoolElementList.size() <= 0)
		{
			LogError("Pool ran out of free elements!");
			return nullptr;
		}

		void* p_freePoolElement = GetFreePoolElement();

		// Update internal data
		usedMemorySize += m_poolElementSize;
		
		++allocationCount;

		return RegisterHandle(p_freePoolElement);
	}

	void PoolAllocator::FreeMemory(const void* const address)
	{
		Assert(reinterpret_cast<uint64Ptr>(address) >= m_pPoolBase &&
			reinterpret_cast<uint64Ptr>(address) <= m_pPoolBase + (m_poolElementCount * m_poolElementSize) + m_poolElementSize,
			"The given memory address '0x%p' is not part of the allocator pool!", reinterpret_cast<const void*>(address));

#ifdef CLEAR_POOL_ELEMENTS_ON_FREE
		memset(const_cast<void*>(address), 0, poolElementSize);
#endif
			
		usedMemorySize -= m_poolElementSize;

		m_pFreePoolElementList.push(reinterpret_cast<uint64Ptr>(address));
		Assert(allocationCount > 0, "Trying to free memory that has not been allocated using this allocator!");
		++freeCount;
	}

	/// <summary>
	/// Allocates a free pool element and aligns the data.
	/// </summary>
	/// <param name="allocSize">The allocation size to allocate. Usually sizeof(T).</param>
	/// <returns>A handle to the pool elements data.</returns>
	HandleInfo* PoolAllocator::AllocAligned(const uint16 allocSize)
	{
		Assert(allocSize * 2 <= m_poolElementSize,
			"The size of an aligned allocated object (%u) may not exceed the pool element size (%u)", allocSize, m_poolElementSize);

		if (m_pFreePoolElementList.size() <= 0)
		{
			LogError("Pool ran out of free elements!");
			return nullptr;
		}

		// If allocation is possible, lookup the next free pool element and return its base ptr.
		void* p_freePoolElement = GetFreePoolElement();

		// Calculate align offset
		uint64Ptr alignOffset = (allocSize - (reinterpret_cast<uint64Ptr>(p_freePoolElement) % allocSize));

		// Offset base pointer for proper alignment
		p_freePoolElement = 
			reinterpret_cast<void*>(reinterpret_cast<uint64Ptr>(p_freePoolElement) + alignOffset);
			
		// Store align offset right before the data
		uint64Ptr p_alignOffsetAddress = reinterpret_cast<uint64Ptr>(p_freePoolElement) - sizeof(AllocationOffset);
		AllocationOffset* p_alignOffset = new (reinterpret_cast<void*>(p_alignOffsetAddress)) AllocationOffset();

		*p_alignOffset = alignOffset;

		// Update internal data
		usedMemorySize += m_poolElementSize;
		++allocationCount;
		
		return RegisterHandle(p_freePoolElement);
	}

	/// <summary>
	/// Frees an aligned pool element from the pool allocator.
	/// </summary>
	/// <param name="address">The address of the element to free.</param>
	void PoolAllocator::FreeAligned(const void* const address)
	{
		Assert(reinterpret_cast<uint64Ptr>(address) >= m_pPoolBase &&
			reinterpret_cast<uint64Ptr>(address) <= m_pPoolBase + (m_poolElementCount * m_poolElementSize) + m_poolElementSize,
			"The given memory address '0x%p' is not part of the allocator pool!", reinterpret_cast<const void*>(address));

		uint64Ptr poolElementBaseAddress = reinterpret_cast<uint64Ptr>(address) - sizeof(AllocationOffset);
		AllocationOffset* p_alignOffset = reinterpret_cast<AllocationOffset*>(poolElementBaseAddress);

		uint64Ptr poolElementAddress = reinterpret_cast<uint64Ptr>(address) - *p_alignOffset;

		// Check if pool element is actually used.
		
#ifdef CLEAR_POOL_ELEMENTS_ON_FREE
		memset(reinterpret_cast<void*>(poolElementAddress), 0, poolElementSize);
#endif
		usedMemorySize -= m_poolElementSize;

		m_pFreePoolElementList.push(poolElementAddress);
		Assert(allocationCount > 0, "Trying to free memory that has not been allocated using this allocator!");
		++freeCount;
	}

	/// <summary>
	/// Resets the pool allocator and makes all pool elements available again.
	/// Does not override the pool elements or zero the memory. Memory is released when deconstructing the pool allocator as a whole.
	/// </summary>
	void PoolAllocator::Clear()
	{
		m_pFreePoolElementList = std::stack<Borealis::Types::uint64Ptr>();

		for(int elementIndex = 0; elementIndex < m_poolElementCount; ++elementIndex)
		{
			m_pFreePoolElementList.emplace(m_pPoolBase + (elementIndex * m_poolElementSize));
		}

		usedMemorySize = 0;
		freeCount = 0;
		allocationCount = 0;
	}

	void* const PoolAllocator::GetFreePoolElement()
    {
		Assert(m_pFreePoolElementList.size() > 0, 
			"Pool allocator ran out of free elements!");

        void* p_poolElement = reinterpret_cast<void*>(m_pFreePoolElementList.top());
		m_pFreePoolElementList.pop();

		return p_poolElement;
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