#pragma once

#include "allocator.h"
#include "../../config.h"
#include "../debug/logger.h"

using namespace Borealis::Types;
using namespace Borealis::Debug;

namespace Borealis::Memory
{
	typedef uint8 AllocationOffset;

	class BOREALIS_API StackAllocator : public BaseAllocator
	{
	public:

		StackAllocator();
		StackAllocator(uint64 size);

		~StackAllocator();

	public:

		template<typename T>
		T* Alloc()
		{
			const uint16 allocSize = sizeof(T);
			Assert(allocSize <= availableMemorySize, "Allocator does not provide enough memory for the requested allocation process.");
			
			T* ptr = new(reinterpret_cast<T*>(stackTopPtr)) T();
			stackTopPtr += static_cast<uint64>(allocSize);
		   
			// Update memory statistics
			usedMemorySize += static_cast<uint64>(allocSize);
			availableMemorySize -= static_cast<uint64>(allocSize);

			BaseAllocator::OnAllocate();

			return ptr;
		}

		template<typename T>
		T* AllocAligned()
		{
			const uint16 allocSize = sizeof(T);
			const uint16 alignedAllocSize = allocSize * 2;	// A maximum of twice the space is needed for properly aligning and storing the offset.

			Assert(allocSize <= availableMemorySize, "Allocator does not provide enough memory for the requested allocation process.");

			// Determine offset for proper alignment. Use one byte to store offset information.
			const uint8 offset = allocSize - (stackTopPtr % allocSize);
			Assert(offset <= 255, "Alignment offset is greater than the available info byte.");
			
			// Store allocation offset due to alignment in the byte before the actual data.
			AllocationOffset* pAllocOffset = reinterpret_cast<AllocationOffset*>(stackTopPtr) + offset - 1;
			*pAllocOffset = offset;

			// Determine pointer and move top of stack up
			T* ptr = new(reinterpret_cast<T*>(stackTopPtr + offset)) T();
			stackTopPtr += static_cast<uint64>(allocSize + offset);

			// Update memory statistics
			usedMemorySize += static_cast<uint64>(allocSize);
			availableMemorySize -= static_cast<uint64>(allocSize + offset);

			BaseAllocator::OnAllocate();

			return ptr;
		}

	   template<typename T>
	   void Free(T* object)
	   {
		   const uint16 freeSize = sizeof(*object);
		   Assert(freeSize <= usedMemorySize, "The size of the type requested for freeing memory is greater than the currently allocated space!");
		   
		   stackTopPtr -= static_cast<uint64>(freeSize);
		   usedMemorySize -= static_cast<uint64>(freeSize);
		   availableMemorySize += static_cast<uint64>(freeSize);

		   BaseAllocator::OnFree();
	   }

	   template<typename T>
	   void FreeAligned(T* object)
	   {
		   const uint16 freeSize = sizeof(*object);
		   Assert(freeSize <= usedMemorySize, "The size of the type requested for freeing memory is greater than the currently allocated space!");
		
		   AllocationOffset* pOffset = reinterpret_cast<AllocationOffset*>(stackTopPtr - freeSize - 1);
		   stackTopPtr -= static_cast<uint64>(freeSize + static_cast<uint8>(*pOffset));
		   usedMemorySize -= static_cast<uint64>(freeSize);
		   availableMemorySize += (static_cast<uint64>(freeSize) + *pOffset);

		   BaseAllocator::OnFree();
	   }

	private:

		uint64 stackBasePtr = 0;
		uint64 stackTopPtr = 0;
	};
}
