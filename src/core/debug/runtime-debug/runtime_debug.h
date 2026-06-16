#pragma once
#include "../../../config.h"
#include "../../helpers/macros.h"
#include "../../types/string_id.h"
#include "../../graphics/helpers/helpers.h"
#include "../../memory/ref_cnt_auto_ptr.h"

#include "IGUI_drawable.h"
#include "debug_category_button.h"
#include "debug_info_label.h"

#include "info_labels.h"
#include "input_debugger.h"
#include "memory_debugger.h"

#include "imgui/imgui.h"

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS

// TODO: Figure out how I can setup and use debug gui only in debug and relwithdebinfo builds
//#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

struct GLFWwindow;

namespace Borealis::Runtime::Debug
{
	
	struct BOREALIS_API RuntimeDebugger : protected IGUIDrawable
	{
		RuntimeDebugger(
			Graphics::Helpers::IBorealisRenderer& renderer
			, Input::InputSystem* pInputSystem
			, Memory::RefCntAutoPtr<Borealis::Graphics::Texture> debugTexAtlas
			, Core::Window* pWindow
		)
			: m_Renderer(renderer), IGUIDrawable(true)
		{ 
			Memory::MemAllocJanitor janitor(Memory::MemAllocatorContext::RENDERING_DEBUG);

			// First, register all debug windows (deriving from IGUIDrawable)
			runtimeGUIDrawables.push_back(Memory::RefCntAutoPtr<InputDebugger>::Allocate(pInputSystem, debugTexAtlas));
			runtimeGUIDrawables.push_back(Memory::RefCntAutoPtr<MemoryDebugger>::Allocate());

			// Then, register category buttons, passing the runtimeGUIDrawables for "click" events
			categoryButtons =
			{
				Memory::RefCntAutoPtr<DebugCategoryButton>::Allocate(ImVec2(100,100), &runtimeGUIDrawables, Types::String("CatA")),
				Memory::RefCntAutoPtr<DebugCategoryButton>::Allocate(ImVec2(100,100), &runtimeGUIDrawables, Types::String("CatB")),
				Memory::RefCntAutoPtr<DebugCategoryButton>::Allocate(ImVec2(100,100), &runtimeGUIDrawables, Types::String("CatC")),
				Memory::RefCntAutoPtr<DebugCategoryButton>::Allocate(ImVec2(100,100), &runtimeGUIDrawables, Types::String("CatD")),
			};

			//Finally, create labels for better overview and customized statistics
			// Add more individual metrics here (e.g. GPU time, vertex count, mesh count, ...)
			debugLabels =
			{
				//new RuntimePauseLabel("Runtime Pause Label", inter_bold, ImVec2(labelHeight, labelHeight)),
				Memory::RefCntAutoPtr<FrameTimeDebugInfoLabel>::Allocate(Types::String("Game Update Time"), inter_bold, ImVec2(260, labelHeight)),
				Memory::RefCntAutoPtr<WindowModeDebugInfoLabel>::Allocate(pWindow, Types::String("Window Mode"), inter_bold, ImVec2(250, labelHeight)),
				//new FrameTimeDebugInfoLabel(Types::String("Game Update Time"), inter_bold, ImVec2(260, labelHeight)),
				//new WindowModeDebugInfoLabel(pWindow, Types::String("Window Mode"), inter_bold, ImVec2(250, labelHeight)),
				//new ImGuiDebugInfoLabel("ImGui Update Time", inter_bold, pImgui_process_time_ms, ImVec2(115, labelHeight)),
				//new PhysicsTimeDebugInfoLabel("Physics Update Time", inter_bold, ImVec2(115, labelHeight)),
				//new ConsoleDebugInfoLabel("Console Info", inter_bold, GetGUIDrawablePtrs(), ImVec2(130, labelHeight)),
				
				// The filter is always the last one
				Memory::RefCntAutoPtr<DebugLabelFilter>::Allocate(Types::String("Filter"), inter_bold, &debugLabels, ImVec2(labelHeight, labelHeight)),
				//new DebugLabelFilter(Types::String("Filter"), inter_bold, &debugLabels, ImVec2(labelHeight, labelHeight)),
			};
		}
		
		~RuntimeDebugger()
		{
			runtimeGUIDrawables.clear();
			categoryButtons.clear();
			debugLabels.clear();

			Detatch();
		}

		BOREALIS_DELETE_COPY_CONSTRUCT(RuntimeDebugger)
		BOREALIS_DELETE_MOVE_CONSTRUCT(RuntimeDebugger)
		BOREALIS_DELETE_COPY_ASSIGN(RuntimeDebugger)
		BOREALIS_DELETE_MOVE_ASSIGN(RuntimeDebugger)

	public:
		void Attatch(GLFWwindow* pWindow);
		void Detatch();

		void UpdateDrawable() override;

	private:
		virtual void OnGui() override;
		void DrawCategoryButtons();
		void DrawDebugInfoLabels();
		
	private:
		bool initialized = false;

		Borealis::Graphics::Helpers::IBorealisRenderer& m_Renderer;
		
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;

		std::vector<Memory::RefCntAutoPtr<IGUIDrawable>> runtimeGUIDrawables = {};
		std::vector<Memory::RefCntAutoPtr<Runtime::Debug::DebugCategoryButton>> categoryButtons = { };
		std::vector<Memory::RefCntAutoPtr<Runtime::Debug::DebugInfoLabel>> debugLabels = { };

		float labelHeight = 34.f;
		static ImDrawData* p_drawData;
	};
}
