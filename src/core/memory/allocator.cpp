#include "allocator.h"

namespace Borealis::Memory
{
	BaseAllocator::BaseAllocator(Borealis::Types::uint64 memorySize)
		: size(memorySize), freeSize(memorySize), usedSize(0)
	{
		
	}
	
	BaseAllocator::~BaseAllocator()
	{
		

		size = 0;
		freeSize = 0;
		usedSize = 0;
	}

	Borealis::Types::uint64 BaseAllocator::GetTotalMemorySize() const
	{
		return size;
	}

	Borealis::Types::uint64 BaseAllocator::GetUsedMemorySize() const
	{
		return usedSize;
	}

	Borealis::Types::uint64 BaseAllocator::GetFreeMemorySize() const
	{
		return freeSize;
	}
}