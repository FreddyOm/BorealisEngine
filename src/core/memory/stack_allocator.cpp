#include "stack_allocator.h"
#include <memory>

using namespace Borealis::Types;

namespace Borealis::Memory
{
	StackAllocator::StackAllocator()
		: BaseAllocator(1024)
	{
		stackBasePtr = reinterpret_cast<uint64Ptr>(malloc(totalMemorySize));
		stackTopPtr = stackBasePtr;
	}

	StackAllocator::StackAllocator(uint64 size)
		:BaseAllocator(size)
	{
		stackBasePtr = reinterpret_cast<uint64Ptr>(malloc(size));
		stackTopPtr = stackBasePtr;
	}

	StackAllocator::~StackAllocator()
	{
		free(reinterpret_cast<void*>(stackBasePtr));
	}

	void StackAllocator::Clear()
	{
		stackTopPtr = stackBasePtr;

		usedMemorySize = 0;
		availableMemorySize = totalMemorySize;
	}
}