#pragma once
#include "IGUI_drawable.h"
#include "imgui/imgui.h"

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
		}

	private:


	};
}