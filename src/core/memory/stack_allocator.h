#pragma once

#include "allocator.h"
#include "../../config.h"
#include "../debug/logger.h"

using namespace Borealis::Types;
using namespace Borealis::Debug;

namespace Borealis::Memory
{
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
			const uint64 allocSize = sizeof(T);
			Assert(allocSize <= freeSize, "Allocator does not provide enough memory for the requested allocation process.");
			
			T* ptr = new(reinterpret_cast<T*>(stackTopPtr)) T();
			stackTopPtr += allocSize;
		   
			// Update memory statistics
			usedSize += allocSize;
			
			return ptr;
		}

		template<typename T>
		T* AllocAligned()
		{
			const uint64 allocSize = sizeof(T);
			const uint64 alignedAllocSize = allocSize * 2;

			Assert(allocSize <= freeSize, "Allocator does not provide enough memory for the requested allocation process.");

			// Determine offset for proper alignment. Use one byte to store offset information.
			const uint8 offset = allocSize - (stackTopPtr % allocSize);
			Assert(offset <= 255, "Alignment offset is greater than the available info byte.");
			
			
			//T* ptr = new(reinterpret_cast<T*>(stackTopPtr)) T();
			//stackTopPtr += allocSize;

			//// Update memory statistics
			//usedSize += allocSize;

			/*return ptr;*/
			return nullptr;
		}

	private:

		uint64 stackBasePtr = 0;
		uint64 stackTopPtr = 0;
	};
}
