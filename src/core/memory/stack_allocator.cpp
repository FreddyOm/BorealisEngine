#include "stack_allocator.h"
#include <memory>
#include "../debug/logger.h"

using namespace Borealis::Types;

namespace Borealis::Memory
{
	StackAllocator::StackAllocator()
		: stackBasePtr(reinterpret_cast<uint64Ptr>(malloc(totalMemorySize))),
		stackTopPtr(stackBasePtr)
	{ 
		totalMemorySize = 1024;
		usedMemorySize = 0;
	}

	StackAllocator::StackAllocator(uint64 size)
		: stackBasePtr(reinterpret_cast<uint64Ptr>(malloc(size))),
		stackTopPtr(stackBasePtr)
	{ 
		totalMemorySize = size;
		usedMemorySize = 0;
	}

	StackAllocator::~StackAllocator()
	{
		Clear();
		free(reinterpret_cast<void*>(stackBasePtr));

		stackTopPtr = 0;
		stackBasePtr = 0;
		totalMemorySize = 0;
	}

	void* StackAllocator::Alloc(const Types::uint16 allocSize)
	{
		Borealis::Debug::Assert(allocSize <= GetAvailableMemorySize(), "Allocator does not provide enough memory for the requested allocation process.");

		//void* ptr = new(reinterpret_cast<void*>(stackTopPtr)) T();
		void* ptr = reinterpret_cast<void*>(stackTopPtr);
		stackTopPtr += static_cast<Borealis::Types::uint64>(allocSize);

		// Update memory statistics
		usedMemorySize += static_cast<Borealis::Types::uint64>(allocSize);
		++allocationCount;

		return ptr;
	}

	void StackAllocator::FreeMemory(const void* address)
	{
		usedMemorySize -= static_cast<uint64>(stackTopPtr - reinterpret_cast<uint64>(address));

		++freeCount;
		stackTopPtr = reinterpret_cast<uint64>(address);
	}

	void* StackAllocator::AllocAligned(const Types::uint16 allocSize)
	{
		const Borealis::Types::uint16 alignedAllocSize = allocSize * 2;	// A maximum of twice the space is needed for properly aligning and storing the offset.

		Borealis::Debug::Assert(allocSize <= GetAvailableMemorySize(), "Allocator does not provide enough memory for the requested allocation process.");

		// Determine offset for proper alignment. Use one byte to store offset information.
		const Borealis::Types::uint8 offset = allocSize - (stackTopPtr % allocSize);
		Borealis::Debug::Assert(offset <= 255, "Alignment offset is greater than the available info byte.");

		// Store allocation offset due to alignment in the byte before the actual data.
		AllocationOffset* pAllocOffset = reinterpret_cast<AllocationOffset*>(stackTopPtr) + offset - 1;
		*pAllocOffset = offset;

		// Determine pointer and move top of stack up
		void* ptr = reinterpret_cast<void*>(stackTopPtr + offset);
		stackTopPtr += static_cast<Borealis::Types::uint64>(allocSize + offset);

		// Update memory statistics
		usedMemorySize += static_cast<Borealis::Types::uint64>(allocSize) + offset;
		++allocationCount;

		return ptr;
	}

	void StackAllocator::FreeAligned(const void* address)
	{
		Debug::LogError("Cannot use the generic \"FreeAlign()\" when using a stack allocator. Use \"FreeToMarker()\" instead.");

		/*AllocationOffset* pOffset = reinterpret_cast<AllocationOffset*>(stackTopPtr - freeSize - 1);
		stackTopPtr -= static_cast<Borealis::Types::uint64>(freeSize + static_cast<Borealis::Types::uint8>(*pOffset));
		usedMemorySize -= static_cast<Borealis::Types::uint64>(freeSize);
		availableMemorySize += (static_cast<Borealis::Types::uint64>(freeSize) + *pOffset);

		Debug::Assert(allocationCount > 0, "Trying to free memory that has not been allocated using this allocator!");
		--allocationCount;*/
	}

	StackAllocMarker StackAllocator::GetMarker() const
	{
		return reinterpret_cast<void*>(stackTopPtr);
	}

	void StackAllocator::Clear()
	{
		stackTopPtr = stackBasePtr;

		usedMemorySize = 0;

		// Reset alloc count since it doesn't say anything meaningful when Clear was called
		freeCount = 0;
		allocationCount = 0;
	}

	Borealis::Types::uint64 StackAllocator::GetTotalMemorySize() const
	{
		return totalMemorySize;
	}
	
	Borealis::Types::uint64 StackAllocator::GetUsedMemorySize() const
	{
		return usedMemorySize;
	}
	
	Borealis::Types::uint64 StackAllocator::GetAvailableMemorySize() const
	{
		return totalMemorySize - usedMemorySize;
	}
	
	Borealis::Types::int8 StackAllocator::GetAllocFreeRatio() const
	{
		return static_cast<int8>(allocationCount - freeCount);
	}
}