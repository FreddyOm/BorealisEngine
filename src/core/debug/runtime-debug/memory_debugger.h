#pragma once
#include "IGUI_drawable.h"
#include "imgui/imgui.h"
#include "../../memory/ref_cnt_auto_ptr.h"
#include "../../memory/pool_allocator.h"
#include "../../memory/heap_allocator.h"
#include "../../memory/stack_allocator.h"
#include <typeinfo>

namespace Borealis::Runtime::Debug
{
	class MemoryDebugger : public IGUIDrawable
	{
	public:

		MemoryDebugger()
			: IGUIDrawable(true)
		{}

		~MemoryDebugger() = default;

		BOREALIS_DELETE_COPY_CONSTRUCT(MemoryDebugger)
		BOREALIS_DELETE_MOVE_CONSTRUCT(MemoryDebugger)
		BOREALIS_DELETE_COPY_ASSIGN(MemoryDebugger)
		BOREALIS_DELETE_MOVE_ASSIGN(MemoryDebugger)

		void UpdateDrawable() override
		{
			if (isOpen)
			{
				ImGui::Begin("Memory Debugger", &isOpen, ImGuiWindowFlags_NoCollapse);
				OnGui();
				ImGui::End();
			}
		}

		void OnGui() override
		{
			ImGui::Text("Memory Debugging stuff!");

			for (Types::uint8 i = 1; i < (Types::uint8)Memory::MemAllocatorContext::NUM_CONTEXTS; ++i)
			{
				DrawMemoryStats((Memory::MemAllocatorContext) i);
			}

			ImGui::Spacing();

		}

	private:

		const char* GetMemoryAllocNameFromContext(const Memory::MemAllocatorContext allocCtxt) const
		{
			switch (allocCtxt)
			{
			case Memory::MemAllocatorContext::FRAME:
				return "Frame Allocator";
			case Memory::MemAllocatorContext::DEBUG:
				return "Debug Allocator";
			case Memory::MemAllocatorContext::STATIC:
				return "Static Allocator";
			case Memory::MemAllocatorContext::RENDERING:
				return "Rendering Allocator";
			case Memory::MemAllocatorContext::RENDERING_DEBUG:
				return "Rendering Debug Allocator";
			default:
				return "Unknown";
			}
			return "Unknown";
		}

		const char* GetMemoryAllocTypeFromContext(const Memory::MemAllocatorContext allocCtxt) const
		{
			const std::type_info& allocatorType = typeid(Memory::GetMemoryAllocator(allocCtxt));
			
			if (dynamic_cast<Memory::HeapAllocator*>(Memory::GetMemoryAllocator(allocCtxt)))
			{
				return "Heap Allocator";
			}
			else if (dynamic_cast<Memory::StackAllocator*>(Memory::GetMemoryAllocator(allocCtxt)))
			{
				return "Stack Allocator";
			}
			else if (dynamic_cast<Memory::PoolAllocator*>(Memory::GetMemoryAllocator(allocCtxt)))
			{
				return "Pool Allocator";
			}
			else
			{
				return "Unknown";
			}

		}

		void DrawMemoryStats(Memory::MemAllocatorContext allocCtxt)
		{
			ImGui::Spacing();

			float frac =
				(float)Memory::GetMemoryAllocator(allocCtxt)->GetUsedMemorySize() /
				(float)Memory::GetMemoryAllocator(allocCtxt)->GetAvailableMemorySize();

			if (ImGui::CollapsingHeader(GetMemoryAllocNameFromContext(allocCtxt)))
			{
				ImGui::Text("Allocator type: %s", GetMemoryAllocTypeFromContext(allocCtxt));

				ImGui::Text("Memory usage: [%li B / %li B]",
					Memory::GetMemoryAllocator(allocCtxt)->GetUsedMemorySize(),
					Memory::GetMemoryAllocator(allocCtxt)->GetAvailableMemorySize());
				
				ImGui::ProgressBar(frac, ImVec2(800, 50));

				ImGui::Spacing();

				ImGui::Text("Current allocations: %i", Memory::GetMemoryAllocator(allocCtxt)->GetAllocFreeRatio());
			}
		}

	};
}