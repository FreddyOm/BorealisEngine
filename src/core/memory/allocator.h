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
		Borealis::Types::uint64 GetFreeMemorySize() const;

	protected:
		
		Borealis::Types::uint64 size;
		Borealis::Types::uint64 usedSize;
		Borealis::Types::uint64 freeSize;
	};
}