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
	std::unordered_map<uint64Ptr, HandleInfo*> g_HandleInfoMap =
		std::unordered_map<uint64Ptr, HandleInfo*>();
	
	// Maybe enter the MemBlockDesc as value and when using the handle, map to its data ptr?
	BOREALIS_API HandleInfo* RegisterHandle(
		void* const p_refCntPtr
#ifdef BOREALIS_DEBUG
		, const std::string& debugInfo
#endif
	)
	{
		Assert(p_refCntPtr != nullptr, "Cannot register nullptr as handle");

		const uint64Ptr handleID = Math::Random::Next64();
		Assert(g_HandleTable.find(handleID) == g_HandleTable.end(),
			"Generated handle is already ");

		// Register the handle and the corresponding data pointer
		g_HandleTable.insert({ handleID, p_refCntPtr });
		// Return the handle info 
#ifdef BOREALIS_DEBUG
		HandleInfo* p_handleInfo = new (g_HandleInfoAllocator.RawAlloc(sizeof(HandleInfo))) HandleInfo(handleID, debugInfo);
#else
		HandleInfo* p_handleInfo = new (g_HandleInfoAllocator.RawAlloc(sizeof(HandleInfo))) HandleInfo(handleID);

#endif
		g_HandleInfoMap.insert({ handleID, p_handleInfo });

		return p_handleInfo;
	}

	BOREALIS_API void UpdateHandle(const uint64Ptr handleId, void* const p_newData)
	{
		if (g_HandleTable.find(handleId) == g_HandleTable.end())
		{
			LogError("Trying to update handle [%u] which could not be found!", handleId);
			return;
		}

		g_HandleTable[handleId] = p_newData;
	}

	BOREALIS_API void RemoveHandle(const Types::uint64Ptr handleId, HandleInfo* const p_hndlInfo)
	{
		if (g_HandleTable.find(handleId) == g_HandleTable.end())
		{
			LogError("Couldn't find handle [%u] in the table!", handleId);
			return;
		}

		// Remove handle from handle table
		g_HandleTable.erase(handleId);
		g_HandleInfoMap.erase(handleId);
		g_HandleInfoAllocator.FreeMemory(p_hndlInfo);
	}

	BOREALIS_API void* const AccessHandleData(const Types::uint64Ptr handleId)
	{
		if (g_HandleTable.find(handleId) == g_HandleTable.end())
		{
			LogError("Couldn't find handle [%u] in the table!", handleId);
			return nullptr;
		}

		return g_HandleTable.find(handleId) == g_HandleTable.end() ? nullptr : g_HandleTable[handleId];
	}

	BOREALIS_API void ReportLiveHandles()
	{
		LogWarning("Reporting %u live handles.", g_HandleTable.size());

		for (const auto& pair : g_HandleTable)
		{
			const char* allocCtxt = "DEFAULT";
			
			switch (g_HandleInfoMap[pair.first]->MemAllocCntxt)
			{
			case MemAllocatorContext::DEFAULT:
				allocCtxt = "DEFAULT";
				break;
			case MemAllocatorContext::RENDERING_DEBUG:
				allocCtxt = "RENDERING_DEBUG";
				break;
			case MemAllocatorContext::RENDERING:
				allocCtxt = "RENDERING";
				break;
			case MemAllocatorContext::CORESYS:
				allocCtxt = "CORESYS";
				break;
			case MemAllocatorContext::FRAME:
				allocCtxt = "FRAME";
				break;
			case MemAllocatorContext::STATIC:
				allocCtxt = "STATIC";
				break;
			case MemAllocatorContext::DEBUG:
				allocCtxt = "DEBUG";
				break;
			default:
				allocCtxt = "UNKNOWN";
				break;
			}
#ifdef BOREALIS_DEBUG
			LogWarning("Handle: [%u] | RefCnt: %i | Allocated on %s as %s", pair.first, g_HandleInfoMap[pair.first]->RefCount, allocCtxt, g_HandleInfoMap[pair.first]->m_DebugInfo.c_str());
#else
			LogWarning("Handle: [%u] | RefCnt: %i | Allocated on %s", pair.first, g_HandleInfoMap[pair.first]->RefCount, allocCtxt);
#endif
		}

		Log("End");
	}


#pragma endregion global handle table

#pragma region memory allocation

	std::stack<MemAllocatorContext> g_memoryAllocatorContext =
		std::stack<MemAllocatorContext>();

	StackAllocator g_frameAllocator(2048);					// 2 KiB
	StackAllocator g_staticAllocator(134217728);			// 128 MiB
	PoolAllocator g_debugAllocator(4096, 65536);			// 256 MiB
	PoolAllocator g_coresysAllocator(4096, 65536);			// 256 MiB
	
	HeapAllocator g_renderingDebugAllocator(67108864);		// 64 MiB
	HeapAllocator g_renderingAllocator(67108864);			// 64 MiB

	HeapAllocator g_defaultAllocator(134217728);			// 128 MiB

	BOREALIS_API IMemoryAllocator* GetMemoryAllocator(const MemAllocatorContext context)
	{
		switch (context)
		{
		case MemAllocatorContext::DEBUG:
		{
			return dynamic_cast<IMemoryAllocator*>(&g_debugAllocator);
		}
		case MemAllocatorContext::CORESYS:
		{
			return dynamic_cast<IMemoryAllocator*>(&g_coresysAllocator);
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
		case MemAllocatorContext::STATIC:
		{
			return dynamic_cast<IMemoryAllocator*>(&g_staticAllocator);
		}
		default:
		{
			return dynamic_cast<IMemoryAllocator*>(&g_defaultAllocator);
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
		Assert(context != MemAllocatorContext::NONE && (Types::int8) context < (Types::int8)MemAllocatorContext::NUM_CONTEXTS, 
			"Cannot push invalid memory allocator context!");

		PushAllocator(context);
	}

	MemAllocJanitor::~MemAllocJanitor()
	{
		Assert(g_memoryAllocatorContext.empty() == false, "Trying to pop an empty memory allocator context stack! Something went wrong!");

		PopAllocator();
	}

#pragma endregion memory allocation

}