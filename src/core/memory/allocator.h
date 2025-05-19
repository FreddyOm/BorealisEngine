#pragma once

#include "../../config.h"
#include "../types/types.h"
#include "../debug/logger.h"
#include <stack>

namespace Borealis::Memory
{
	typedef Types::uint8 AllocationOffset;

	struct MemBlockDescription	// 16 bytes
	{
		MemBlockDescription(void* p_data, const Types::uint16 size, const Types::uint16 alignOffset = 0)
			: p_Data(p_data), Size(size), AlignOffset(alignOffset)
		{ }
		
		//RefCntAutoPtr<void>* p_refCntPtr = nullptr;			//16 bytes
		void* p_Data = nullptr;							// 8 bytes
		Types::uint16 Size = 0;							// 2 bytes
		Types::uint16 AlignOffset = 0;					// 2 bytes
		Types::uint8 Padding[4] = {};					// 4 bytes
		
		
		inline Types::uint64Ptr GetMemBlockStart() const
		{
			return reinterpret_cast<Types::uint64Ptr>(p_Data) - AlignOffset - sizeof(MemBlockDescription);
		}
		
		inline Types::uint64Ptr GetMemBlockEnd() const
		{
			return reinterpret_cast<Types::uint64Ptr>(p_Data) + Size;
		}
	};

	enum class BOREALIS_API MemAllocatorContext : Types::int8
	{
		// Default none value
		NONE = -1,

		// Memory dedicated to debug systems, like ...
		DEBUG = 0,

		// Memory dedicated to rendering
		RENDERING = 1,

		// Memory dedicated to per-frame allocations
		FRAME = 2,		

		// Static and persistent data that reside in 
		// memory and don't need to be deallocated
		STATIC = 3,

	};

	void BOREALIS_API PushAllocator(const MemAllocatorContext context);
	void BOREALIS_API PopAllocator();

	extern BOREALIS_API std::stack<MemAllocatorContext> g_memoryAllocatorContext;

	struct MemAllocJanitor
	{
		explicit MemAllocJanitor(const MemAllocatorContext context)
		{
			Debug::Assert(context != MemAllocatorContext::NONE,
				"Cannot push none memory allocator context!");
			
			PushAllocator(context);
		}

		~MemAllocJanitor()
		{
			Debug::Assert(g_memoryAllocatorContext.empty() == false, 
				"Trying to pop an empty memory allocator context stack! Something went wrong!");

			PopAllocator();
		}
	};
	
	class BOREALIS_API IMemoryAllocator
	{
	public:
		virtual ~IMemoryAllocator() = default;
		virtual void* Alloc(const Types::uint16 allocSize) = 0;
		virtual void* AllocAligned(const Types::uint16 allocSize) = 0;

		virtual void FreeMemory(const void* address) = 0;
		virtual void FreeAligned(const void* address) = 0;

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

	struct StackAllocator;
	struct PoolAllocator;

	BOREALIS_API extern StackAllocator g_frameAllocator;
	BOREALIS_API extern StackAllocator g_staticAllocator;
	BOREALIS_API extern PoolAllocator g_debugAllocator;

	BOREALIS_API IMemoryAllocator* GetMemoryAllocator(const MemAllocatorContext context);

	template<typename T>
	T* Allocate()
	{
		if (g_memoryAllocatorContext.empty())
		{
			Debug::LogError("No memory allocator assigned for allocation! Use a MemAllocJanitor to push an allocator context!");
			return nullptr;
		}

		void* ptr = GetMemoryAllocator(g_memoryAllocatorContext.top())->Alloc(sizeof(T));
		if (ptr) {
			return new (ptr) T();
		}

		return nullptr;
	}

	template<typename T>
	T* AllocAligned()
	{
		if (g_memoryAllocatorContext.empty())
			return nullptr;

		void* ptr = GetMemoryAllocator(g_memoryAllocatorContext.top())->AllocAligned(sizeof(T));
		if (ptr) {
			return new (ptr) T();
		}

		return nullptr;
	}

	template<typename T>
	void Free(const T* address)
	{
		if (g_memoryAllocatorContext.empty())
		{
			Debug::LogError("No memory allocator pushed to the allocator stack. Use \"PushAllocator()\" to add an allocator to the global stack.");
			return;
		}
		
		GetMemoryAllocator(g_memoryAllocatorContext.top())->FreeMemory(address);
	}

	template<typename T>
	void FreeAligned(const T* address)
	{
		if (g_memoryAllocatorContext.empty())
		{
			Debug::LogError("No memory allocator pushed to the allocator stack. Use \"PushAllocator()\" to add an allocator to the global stack.");
			return;
		}
		
		GetMemoryAllocator(g_memoryAllocatorContext.top())->FreeAligned(address);
	}

	void BOREALIS_API FlushAllocator();
}