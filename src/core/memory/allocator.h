#pragma once

#include "../../config.h"
#include "../types/types.h"
#include <cstring>

namespace Borealis::Memory
{
	struct HandleInfo;

	class BOREALIS_API IMemoryAllocator
	{
	public:
		virtual ~IMemoryAllocator() = default;
		virtual HandleInfo* Alloc(const Types::uint16 allocSize) = 0;
		virtual HandleInfo* AllocAligned(const Types::uint16 allocSize) = 0;

		virtual void FreeMemory(const void* const address) = 0;
		virtual void FreeAligned(const void* const address) = 0;

		virtual void Clear() = 0;

	public: 

		virtual Borealis::Types::uint64 GetTotalMemorySize() const  = 0;
		virtual Borealis::Types::uint64 GetUsedMemorySize() const = 0;
		virtual Borealis::Types::uint64 GetAvailableMemorySize() const = 0;
		virtual Borealis::Types::int8 GetAllocFreeRatio() const = 0;

	protected:

		Borealis::Types::uint64 totalMemorySize = 0;
		Borealis::Types::uint64 usedMemorySize = 0;
		Borealis::Types::uint64 allocationCount = 0;
		Borealis::Types::uint64 freeCount = 0;
	};
}