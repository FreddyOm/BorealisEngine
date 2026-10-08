#pragma once

#include "../../config.h"
#include "../helpers/macros.h"
#include "allocator.h"

namespace Borealis::Memory
{
    typedef void* StackAllocMarker;

    class BOREALIS_API StackAllocator : public IMemoryAllocator
    {
       public:
        StackAllocator();
        StackAllocator(Borealis::Types::uint64 size);

        ~StackAllocator() override;

        BOREALIS_DELETE_COPY_CONSTRUCT(StackAllocator)
        BOREALIS_DELETE_MOVE_CONSTRUCT(StackAllocator)
        BOREALIS_DELETE_COPY_ASSIGN(StackAllocator)
        BOREALIS_DELETE_MOVE_ASSIGN(StackAllocator)

       public:
#ifdef BOREALIS_DEBUG
        HandleInfo* Alloc(const Types::uint16 allocSize, const std::string& debugInfo = "") override;
        HandleInfo* AllocAligned(const Types::uint16 allocSize, const std::string& debugInfo = "") override;
#else
        HandleInfo* Alloc(const Types::uint16 allocSize) override;
        HandleInfo* AllocAligned(const Types::uint16 allocSize) override;
#endif

        void FreeMemory(const void* const address) override;
        void FreeAligned(const void* const address) override;

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
}    // namespace Borealis::Memory