#pragma once

#include "allocator.h"
#include "../../config.h"
#include "../debug/logger.h"

namespace Borealis::Memory
{
	typedef Borealis::Types::uint8 AllocationOffset;

	class BOREALIS_API StackAllocator : public BaseAllocator
	{
	public:

		StackAllocator();
		StackAllocator(Borealis::Types::uint64 size);

		~StackAllocator();

	public:

		template<typename T>
		T* Alloc()
		{
			const Borealis::Types::uint16 allocSize = sizeof(T);
			Borealis::Debug::Assert(allocSize <= availableMemorySize, "Allocator does not provide enough memory for the requested allocation process.");
			
			T* ptr = new(reinterpret_cast<T*>(stackTopPtr)) T();
			stackTopPtr += static_cast<Borealis::Types::uint64>(allocSize);
		   
			// Update memory statistics
			usedMemorySize += static_cast<Borealis::Types::uint64>(allocSize);
			availableMemorySize -= static_cast<Borealis::Types::uint64>(allocSize);

			BaseAllocator::OnAllocate();

			return ptr;
		}

		template<typename T>
		T* AllocAligned()
		{
			const Borealis::Types::uint16 allocSize = sizeof(T);
			const Borealis::Types::uint16 alignedAllocSize = allocSize * 2;	// A maximum of twice the space is needed for properly aligning and storing the offset.

			Borealis::Debug::Assert(allocSize <= availableMemorySize, "Allocator does not provide enough memory for the requested allocation process.");

			// Determine offset for proper alignment. Use one byte to store offset information.
			const Borealis::Types::uint8 offset = allocSize - (stackTopPtr % allocSize);
			Borealis::Debug::Assert(offset <= 255, "Alignment offset is greater than the available info byte.");
			
			// Store allocation offset due to alignment in the byte before the actual data.
			AllocationOffset* pAllocOffset = reinterpret_cast<AllocationOffset*>(stackTopPtr) + offset - 1;
			*pAllocOffset = offset;

			// Determine pointer and move top of stack up
			T* ptr = new(reinterpret_cast<T*>(stackTopPtr + offset)) T();
			stackTopPtr += static_cast<Borealis::Types::uint64>(allocSize + offset);

			// Update memory statistics
			usedMemorySize += static_cast<Borealis::Types::uint64>(allocSize);
			availableMemorySize -= static_cast<Borealis::Types::uint64>(allocSize + offset);

			BaseAllocator::OnAllocate();

			return ptr;
		}

	   template<typename T>
	   void Free(T* object)
	   {
		   const Borealis::Types::uint16 freeSize = sizeof(*object);
		   Borealis::Debug::Assert(freeSize <= usedMemorySize, "The size of the type requested for freeing memory is greater than the currently allocated space!");
		   
		   stackTopPtr -= static_cast<Borealis::Types::uint64>(freeSize);
		   usedMemorySize -= static_cast<Borealis::Types::uint64>(freeSize);
		   availableMemorySize += static_cast<Borealis::Types::uint64>(freeSize);

		   BaseAllocator::OnFree();
	   }

	   template<typename T>
	   void FreeAligned(T* object)
	   {
		   const Borealis::Types::uint16 freeSize = sizeof(*object);
		   Borealis::Debug::Assert(freeSize <= usedMemorySize, "The size of the type requested for freeing memory is greater than the currently allocated space!");
		
		   AllocationOffset* pOffset = reinterpret_cast<AllocationOffset*>(stackTopPtr - freeSize - 1);
		   stackTopPtr -= static_cast<Borealis::Types::uint64>(freeSize + static_cast<Borealis::Types::uint8>(*pOffset));
		   usedMemorySize -= static_cast<Borealis::Types::uint64>(freeSize);
		   availableMemorySize += (static_cast<Borealis::Types::uint64>(freeSize) + *pOffset);

		   BaseAllocator::OnFree();
	   }

	   void Clear();

	private:

		Borealis::Types::uint64 stackBasePtr = 0;
		Borealis::Types::uint64 stackTopPtr = 0;
	};
}
