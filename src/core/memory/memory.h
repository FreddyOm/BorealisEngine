#pragma once

#include "../../config.h"
#include "../types/types.h"
#include "../debug/logger.h"
#include "allocator.h"

#include <stack>
#include <unordered_map>

namespace Borealis::Memory
{
#pragma region forward declarations
	
	struct PoolAllocator;

#pragma endregion forward decleration

	typedef Types::uint8 AllocationOffset;

	enum class BOREALIS_API MemAllocatorContext : Types::int8
	{
		// Default none value
		NONE = -1,

		// Memory dedicated to debug systems, like ...
		DEBUG = 0,

		// Memory dedicated to rendering
		RENDERING = 1,

		// Memory dedicated to rendering
		RENDERING_DEBUG = 2,

		// Memory dedicated to per-frame allocations
		FRAME = 3,

		// Static and persistent data that reside in 
		// memory and don't need to be deallocated
		STATIC = 4,

	};
	extern BOREALIS_API std::stack<MemAllocatorContext> g_memoryAllocatorContext;


#pragma region handle table

	struct HandleInfo	// 16 bytes
	{
		explicit HandleInfo(const Types::uint64Ptr handleId, void* const p_data)
			: HandleId(handleId), /*p_Data(p_data),*/ RefCount(1), MemAllocCntxt(g_memoryAllocatorContext.empty() ? MemAllocatorContext::STATIC : g_memoryAllocatorContext.top())
		{ }

		Types::uint64Ptr HandleId = 0;		// 8 bytes
		Types::int32 RefCount = 0;			// 4 bytes
		MemAllocatorContext MemAllocCntxt = MemAllocatorContext::DEBUG;	// 1 bytes
		Types::int8 Padding[3]{};			// 3 bytes
	};

	extern BOREALIS_API PoolAllocator g_HandleInfoAllocator;
	extern BOREALIS_API std::unordered_map<Types::uint64Ptr, void*> g_HandleTable;

	BOREALIS_API HandleInfo* RegisterHandle(void* const p_dataPtr);
	BOREALIS_API void UpdateHandle(const Types::uint64Ptr handleId, void* const p_newData);
	BOREALIS_API void RemoveHandle(const Types::uint64Ptr handleId, HandleInfo* const p_hndlInfo);
	BOREALIS_API void* const AccessHandleData(const Types::uint64Ptr handleId);

#pragma endregion handle table

#pragma region global memory allocation

	struct BOREALIS_API MemAllocJanitor
	{
		explicit MemAllocJanitor(const MemAllocatorContext context);
		~MemAllocJanitor();
	};

	BOREALIS_API IMemoryAllocator* GetMemoryAllocator(const MemAllocatorContext context);
	
	BOREALIS_API void PushAllocator(const MemAllocatorContext context);
	BOREALIS_API void PopAllocator();
	BOREALIS_API void FlushAllocator();


	template<typename T>
	T* Allocate()
	{
		if (g_memoryAllocatorContext.empty())
		{
			LogError("No memory allocator assigned for allocation! Use a MemAllocJanitor to push an allocator context!");
			return nullptr;
		}

		HandleInfo* p_hndl = GetMemoryAllocator(g_memoryAllocatorContext.top())->Alloc(sizeof(T));
		Assert(p_hndl != nullptr, "Failed to allocate memory!");
		if (p_hndl) 
		{
			return new (AccessHandleData(p_hndl->HandleId)) T();
		}

		return nullptr;
	}

	template<typename T>
	T* AllocAligned()
	{
		if (g_memoryAllocatorContext.empty())
			return nullptr;

		HandleInfo* p_hndl = GetMemoryAllocator(g_memoryAllocatorContext.top())->AllocAligned(sizeof(T));
		Assert(p_hndl != nullptr, "Failed to allocate memory!");
		if (p_hndl) 
		{
			return new (AccessHandleData(p_hndl->HandleId)) T();
		}

		return nullptr;
	}

	template<typename T>
	void Free(const T* address)
	{
		if (g_memoryAllocatorContext.empty())
		{
			LogError("No memory allocator pushed to the allocator stack. Use \"PushAllocator()\" to add an allocator to the global stack.");
			return;
		}

		GetMemoryAllocator(g_memoryAllocatorContext.top())->FreeMemory(address);
	}

	template<typename T>
	void FreeAligned(const T* address)
	{
		if (g_memoryAllocatorContext.empty())
		{
			LogError("No memory allocator pushed to the allocator stack. Use \"PushAllocator()\" to add an allocator to the global stack.");
			return;
		}

		GetMemoryAllocator(g_memoryAllocatorContext.top())->FreeAligned(address);
	}

#pragma endregion global memory allocation

}