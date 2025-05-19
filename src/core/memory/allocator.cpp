#include "allocator.h"
#include "stack_allocator.h"
#include "pool_allocator.h"
#include "heap_allocator.h"

using namespace Borealis::Debug;
using namespace Borealis::Types;

namespace Borealis::Memory
{
	std::stack<MemAllocatorContext> g_memoryAllocatorContext =
		std::stack<MemAllocatorContext>();

	StackAllocator g_frameAllocator(1048576);				// 1 MiB
	StackAllocator g_staticAllocator(134217728);			// 128 MiB
	PoolAllocator g_debugAllocator(4096, 65536);			// 256 MiB
	HeapAllocator g_renderingAllocator(134217728 * 8);		// 1 GiB 


	IMemoryAllocator* GetMemoryAllocator(const MemAllocatorContext context)
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
			Debug::LogWarning("Cannot flush allocator because no allocator is currently assigned!");
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
}