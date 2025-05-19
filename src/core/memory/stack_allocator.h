#pragma once

#include "allocator.h"
#include "../../config.h"

namespace Borealis::Memory
{
	typedef void* StackAllocMarker;

	class BOREALIS_API StackAllocator : public IMemoryAllocator
	{
	public:

		StackAllocator();
		StackAllocator(Borealis::Types::uint64 size);

		~StackAllocator() override;

	public:

		void* Alloc(const Types::uint16 allocSize) override;
		void FreeMemory(const void* address) override;
		void* AllocAligned(const Types::uint16 allocSize) override;
		void FreeAligned(const void* address) override;

		StackAllocMarker GetMarker() const;

		void Clear() override;

		Borealis::Types::uint64 GetTotalMemorySize() const override;
		Borealis::Types::uint64 GetUsedMemorySize() const override;
		Borealis::Types::uint64 GetAvailableMemorySize() const override;
		Borealis::Types::int8 GetAllocFreeRatio() const override;

	private:

		Borealis::Types::uint64 stackBasePtr = 0;
		Borealis::Types::uint64 stackTopPtr = 0;
	};
}
