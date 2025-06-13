#include "memory.h"
#include "heap_allocator.h"
#include "stack_allocator.h"
#include "pool_allocator.h"
#include "../math/random.h"

using namespace Borealis::Types;
using namespace Borealis::Debug;

namespace Borealis::Memory
{

#pragma region global handle table

	PoolAllocator g_HandleInfoAllocator = PoolAllocator(4096, sizeof(HandleInfo));

	std::unordered_map<uint64Ptr, void*> g_HandleTable =
		std::unordered_map<uint64Ptr, void*>();
	
	// Maybe enter the MemBlockDesc as value and when using the handle, map to its data ptr?
	BOREALIS_API HandleInfo* RegisterHandle(void* const p_refCntPtr)
	{
		Assert(p_refCntPtr != nullptr, "Cannot register nullptr as handle");

		const uint64Ptr handleID = Math::Random::Next64();
		Assert(g_HandleTable.find(handleID) == g_HandleTable.end(),
			"Generated handle is already ");

		// Register the handle and the corresponding data pointer
		g_HandleTable.insert({ handleID, p_refCntPtr });

		// Return the handle info 
		return new (g_HandleInfoAllocator.RawAlloc(sizeof(HandleInfo))) HandleInfo(handleID, p_refCntPtr);
	}

	BOREALIS_API void UpdateHandle(const uint64Ptr handleId, void* const p_newData)
	{
		Assert(g_HandleTable.find(handleId) != g_HandleTable.end(), 
			"Handle ID [%u] could not be found in the handle table!", handleId);

		g_HandleTable[handleId] = p_newData;
	}

	BOREALIS_API void RemoveHandle(const Types::uint64Ptr handleId, HandleInfo* const p_hndlInfo)
	{
		Assert(g_HandleTable.find(handleId) != g_HandleTable.end(),
			"Couldn't find handle [%u] in the table!", handleId);

		// Remove handle from handle table
		g_HandleTable.erase(handleId);
		g_HandleInfoAllocator.FreeMemory(p_hndlInfo);
	}

	BOREALIS_API void* const AccessHandleData(const Types::uint64Ptr handleId)
	{
		Assert(g_HandleTable.find(handleId) != g_HandleTable.end(),
			"Handle ID [%u] could not be found in the handle table!", handleId);

		return g_HandleTable[handleId];
	}

#pragma endregion global handle table

#pragma region memory allocation

	std::stack<MemAllocatorContext> g_memoryAllocatorContext =
		std::stack<MemAllocatorContext>();

	StackAllocator g_frameAllocator(1048576);				// 1 MiB
	StackAllocator g_staticAllocator(134217728);			// 128 MiB
	PoolAllocator g_debugAllocator(4096, 65536);			// 256 MiB
	HeapAllocator g_renderingAllocator(134217728 * 8);		// 1 GiB 
	HeapAllocator g_renderingDebugAllocator(134217728 * 4);	// 512 MiB 

	BOREALIS_API IMemoryAllocator* GetMemoryAllocator(const MemAllocatorContext context)
	{
		switch (context)
		{
		case MemAllocatorContext::DEBUG:
		{
			return dynamic_cast<IMemoryAllocator*>(&g_debugAllocator);
		}
		case MemAllocatorContext::RENDERING:
		{
			return dynamic_cast<IMemoryAllocator*>(&g_renderingAllocator);
		}
		case MemAllocatorContext::RENDERING_DEBUG:
		{
			return dynamic_cast<IMemoryAllocator*>(&g_renderingDebugAllocator);
		}
		case MemAllocatorContext::FRAME:
		{
			return dynamic_cast<IMemoryAllocator*>(&g_frameAllocator);
		}
		default:
		case MemAllocatorContext::STATIC:
		{
			return dynamic_cast<IMemoryAllocator*>(&g_staticAllocator);
		}
		}
	}

	void FlushAllocator()
	{
		if (g_memoryAllocatorContext.empty())
		{
			LogWarning("Cannot flush allocator because no allocator is currently assigned!");
			return;
		}

		GetMemoryAllocator(g_memoryAllocatorContext.top())->Clear();
	}

	void PushAllocator(const MemAllocatorContext context)
	{
		g_memoryAllocatorContext.emplace(context);
	}

	void PopAllocator()
	{
		g_memoryAllocatorContext.pop();
	}
	
	MemAllocJanitor::MemAllocJanitor(const MemAllocatorContext context)
	{
		Assert(context != MemAllocatorContext::NONE, "Cannot push none memory allocator context!");

		PushAllocator(context);
	}

	MemAllocJanitor::~MemAllocJanitor()
	{
		Assert(g_memoryAllocatorContext.empty() == false, "Trying to pop an empty memory allocator context stack! Something went wrong!");

		PopAllocator();
	}

#pragma endregion memory allocation

}