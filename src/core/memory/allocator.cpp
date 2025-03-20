#include "allocator.h"
#include "../debug/logger.h"

using namespace Borealis::Debug;
using namespace Borealis::Types;

namespace Borealis::Memory
{
	BaseAllocator::BaseAllocator(uint64 memorySize)
		: totalMemorySize(memorySize), availableMemorySize(memorySize), usedMemorySize(0)
	{ }
	
	BaseAllocator::~BaseAllocator()
	{
		totalMemorySize = 0;
		availableMemorySize = 0;
		usedMemorySize = 0;
	}

	void BaseAllocator::OnAllocate()
	{
		++allocationCount;
	}

	void BaseAllocator::OnFree()
	{
		Assert(allocationCount > 0, "Trying to free memory that has not been allocated using this allocator!");
		--allocationCount;
	}

	uint64 BaseAllocator::GetTotalMemorySize() const
	{
		return totalMemorySize;
	}

	uint64 BaseAllocator::GetUsedMemorySize() const
	{
		return usedMemorySize;
	}

	uint64 BaseAllocator::GetFreeMemorySize() const
	{
		return availableMemorySize;
	}
	
	int8 BaseAllocator::GetAllocFreeRatio() const
	{
		return static_cast<int8>(allocationCount - freeCount);
	}
}