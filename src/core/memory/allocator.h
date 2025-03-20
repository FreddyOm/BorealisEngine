#pragma once

#include "../../config.h"
#include "../types/types.h"

namespace Borealis::Memory
{
	class BOREALIS_API BaseAllocator
	{
	public:

		BaseAllocator(Borealis::Types::uint64 memorySize);
		~BaseAllocator();

		// No copy / move available for now!
		BaseAllocator(const BaseAllocator& other) = delete;
		BaseAllocator(BaseAllocator&& other) = delete;

		BaseAllocator operator=(const BaseAllocator& other) = delete;
		BaseAllocator operator=(BaseAllocator&& other) = delete;

	public:
		
		Borealis::Types::uint64 GetTotalMemorySize() const;
		Borealis::Types::uint64 GetUsedMemorySize() const;
		Borealis::Types::uint64 GetAvailableMemorySize() const;
		Borealis::Types::int8 GetAllocFreeRatio() const;

	protected:

		void OnAllocate();
		void OnFree();

	protected:
		
		Borealis::Types::uint64 totalMemorySize = 0;
		Borealis::Types::uint64 usedMemorySize = 0;
		Borealis::Types::uint64 availableMemorySize = 0;
		Borealis::Types::uint64 allocationCount = 0;
		Borealis::Types::uint64 freeCount = 0;
	};
}