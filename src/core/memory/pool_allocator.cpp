#include "pool_allocator.h"

namespace Borealis::Memory
{
	using namespace Borealis::Types;
	using namespace Borealis::Debug;
	using namespace Borealis::Helpers;
	
	PoolAllocator::PoolAllocator(uint32 poolElementCount, uint64 elementSize)
		: poolElementCount(poolElementCount), poolElementSize(elementSize), freePoolElements(poolElementCount), 
		BaseAllocator(poolElementCount * elementSize)
	{ 
		Assert(poolElementCount > 0, "The number of pool elements may not be 0!");
		Assert(elementSize > 0, "The pool element size may not be 0!");

		// @TODO: Align memory to 16 / 32 / 64 (?) bytes
		// @TODO: Replace the byte-sized lookup base for a bitmask buffer which is more memory efficient. 
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

	void PoolAllocator::Clear()
	{
		memset(p_lookupBase, 0, poolElementCount);

		freePoolElements = poolElementCount;
		usedMemorySize = 0;
		availableMemorySize = totalMemorySize;
		BaseAllocator::freeCount = 0;
		BaseAllocator::allocationCount = 0;
	}
}