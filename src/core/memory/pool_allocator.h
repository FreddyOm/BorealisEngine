#pragma once

#include "allocator.h"
#include "../../config.h"
#include "../debug/logger.h"
#include "../../helpers/helpers.h"

#ifndef CLEAR_POOL_ELEMENTS_ON_FREE	// Resets the memory whenever Free() is called.
//#define CLEAR_POOL_ELEMENTS_ON_FREE
#endif

namespace Borealis::Memory
{
	class BOREALIS_API PoolAllocator : public BaseAllocator
	{
	public:

		PoolAllocator(Borealis::Types::uint32 poolElementNum, 
			Borealis::Types::uint64 elementSize = 65536);	// Default elementSize is 64 kiB
		PoolAllocator(const PoolAllocator& other) = delete;
		PoolAllocator(PoolAllocator&& other) noexcept = delete;

		~PoolAllocator();

		void operator=(const PoolAllocator& other) = delete;
		void operator=(PoolAllocator&& other) noexcept = delete;

	public:

		template<typename T>
		T* Alloc()
		{
			Debug::Assert(sizeof(T) <= poolElementSize, 
				"The size of an allocated object (%u) may not exceed the pool element size (%u)", sizeof(T), poolElementSize);

			if (freePoolElements <= 0)
			{
				Debug::LogError("Pool ran out of free elements!");
				return nullptr;
			}
			
			void* p_freePoolElement = nullptr;
			
			// If allocation is possible, lookup the next free pool element and return its base ptr.
			for (Types::int32 poolElementIndex = 0; poolElementIndex < poolElementCount; ++poolElementIndex)
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
			availableMemorySize -= poolElementSize;
			usedMemorySize += poolElementSize;
			BaseAllocator::OnAllocate();

			return (T*)p_freePoolElement;
		}

		// @TODO: Add aligned allocation

		template<typename T>
		void Free(const T* address)
		{
			Debug::Assert(reinterpret_cast<Types::uint64Ptr>(address) >= p_poolBase && 
				reinterpret_cast<Types::uint64Ptr>(address) <= p_poolBase + (poolElementCount * poolElementSize) + poolElementSize,
				"The given memory address '0x%p' is not part of the allocator pool!", reinterpret_cast<const void*>(address));

			Types::int32 poolIndex = (reinterpret_cast<Types::uint64Ptr>(address) - p_poolBase) / poolElementSize;
			Debug::Assert(poolIndex >= 0 && poolIndex < poolElementCount, 
				"Pool index invalid! The index %i is not within the range [0, %i]!", poolIndex, poolElementCount - 1);
			
			Debug::Assert(p_lookupBase[poolIndex], 
				"Pool index (%i) points to free pool element. Cannot free an already free pool element!", poolIndex);

			// Check if pool element is actually used.
			if (poolIndex < poolElementCount && p_lookupBase[poolIndex])
			{

#ifdef CLEAR_POOL_ELEMENTS_ON_FREE
				memset(reinterpret_cas<void*>(p_poolBase + (poolElementSize * poolIndex)), 0, poolElementSize);
#endif
				usedMemorySize -= poolElementSize;
				availableMemorySize += poolElementSize;
				++freePoolElements;

				p_lookupBase[poolIndex] = false;
				BaseAllocator::OnFree();
			}
			
		}
		
		void Clear();

	private:

		Borealis::Types::uint32 poolElementCount = 0;
		Borealis::Types::uint64 poolElementSize = 0;

		Borealis::Types::uint32 freePoolElements = 0;

		Borealis::Types::uint64Ptr p_allocatorBase = 0;
		Borealis::Types::uint64Ptr p_poolBase = 0;
		bool* p_lookupBase = nullptr;
	};
}