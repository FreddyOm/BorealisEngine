#pragma once

#include "../../config.h"
#include "allocator.h"
#include "../types/types.h"

#include <stack>

#ifndef CLEAR_POOL_ELEMENTS_ON_FREE	// Resets the memory whenever Free() is called.
//#define CLEAR_POOL_ELEMENTS_ON_FREE
#endif

// TODO: Provide both memory efficient pool alloc (using bitmask for empty pool elements) and fast pool alloc (using stack)
// 		 bitmask means small memory footprint, stack means 8-64x size per pool element.

namespace Borealis::Memory
{
	class BOREALIS_API PoolAllocator : public IMemoryAllocator
	{
	public:

		PoolAllocator(Borealis::Types::uint32 poolElementNum, 
			Borealis::Types::uint64 elementSize = 65536);	// Default elementSize is 64 kiB
		PoolAllocator(const PoolAllocator& other) = delete;
		PoolAllocator(PoolAllocator&& other) noexcept = delete;

		~PoolAllocator() override;

		void operator=(const PoolAllocator& other) = delete;
		void operator=(PoolAllocator&& other) noexcept = delete;

	public: 

		void* RawAlloc(const Types::uint16 allocSize);
		HandleInfo* Alloc(const Types::uint16 allocSize) override;
		void FreeMemory(const void* const address) override;

		HandleInfo* AllocAligned(const Types::uint16 allocSize) override;
		void FreeAligned(const void* const address) override;

		void Clear() override;

		Borealis::Types::uint64 GetTotalMemorySize() const override;
		Borealis::Types::uint64 GetUsedMemorySize() const override;
		Borealis::Types::uint64 GetAvailableMemorySize() const override;
		Borealis::Types::int8 GetAllocFreeRatio() const override;	

	protected:

		void* const GetFreePoolElement();		

	private:

		Borealis::Types::uint32 poolElementCount = 0;
		Borealis::Types::uint64 poolElementSize = 0;

		Borealis::Types::uint64Ptr p_poolBase = 0;
		//Borealis::Types::uint64Ptr p_allocatorBase = 0;
		// bool* p_lookupBase = nullptr;

		std::stack<Borealis::Types::uint64Ptr> p_freePoolElementList;
	};
}