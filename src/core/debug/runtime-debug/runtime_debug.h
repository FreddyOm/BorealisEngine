#pragma once
#include "../../../config.h"
#include "../../types/string_id.h"
#include "../../graphics/helpers/helpers.h"

#include "IGUI_drawable.h"
#include "debug_category_button.h"
#include "debug_info_label.h"
#include "input_debugger.h"

#include "imgui/imgui.h"

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS

// TODO: Figure out how I can setup and use debug gui only in debug and relwithdebinfo builds
//#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

struct GLFWwindow;

namespace Borealis::Runtime::Debug
{
	
	struct BOREALIS_API RuntimeDebugger : protected IGUIDrawable
	{
		RuntimeDebugger(Graphics::Helpers::IBorealisRenderer& renderer, Input::InputSystem* pInputSystem)
			: m_Renderer(renderer), IGUIDrawable(true)
		{ 

			// First, register all debug windows (deriving from IGUIDrawable)
			runtimeGUIDrawables.push_back(new InputDebugger(pInputSystem));
			//runtimeGUIDrawables.push_back(new InputDebugger());

			// Then, register category buttons, passing the runtimeGUIDrawables for "click" events
			categoryButtons =
			{
				DebugCategoryButton(ImVec2(100,100), &runtimeGUIDrawables, Types::String("CatA")),
				DebugCategoryButton(ImVec2(100,100), &runtimeGUIDrawables, Types::String("CatB")),
				DebugCategoryButton(ImVec2(100,100), &runtimeGUIDrawables, Types::String("CatC")),
				DebugCategoryButton(ImVec2(100,100), &runtimeGUIDrawables, Types::String("CatD")),
			};

			//Finally, create labels for better overview and customized statistics
			// Add more individual metrics here (e.g. GPU time, vertex count, mesh count, ...)
			debugLabels =
			{
				//new RuntimePauseLabel("Runtime Pause Label", inter_bold, ImVec2(labelHeight, labelHeight)),
				//new FrameTimeDebugInfoLabel("Game Update Time", inter_bold, ImVec2(130, labelHeight)),
				//new ImGuiDebugInfoLabel("ImGui Update Time", inter_bold, pImgui_process_time_ms, ImVec2(115, labelHeight)),
				//new PhysicsTimeDebugInfoLabel("Physics Update Time", inter_bold, ImVec2(115, labelHeight)),
				//new ConsoleDebugInfoLabel("Console Info", inter_bold, GetGUIDrawablePtrs(), ImVec2(130, labelHeight)),
				//
				//// The filter is always the last one
				//new DebugLabelFilter("Filter", inter_bold, &labels, ImVec2(20, labelHeight)),
			};
		}
		
		~RuntimeDebugger()
		{
			Detatch();

			for (auto* runtimeDebugWindow : runtimeGUIDrawables)
			{
				delete runtimeDebugWindow;
			}

			runtimeGUIDrawables.clear();

			for (auto* label : debugLabels)
			{
				delete label;
			}

			debugLabels.clear();
		}

	public:
		void Attatch(GLFWwindow* pWindow);
		void Detatch();

		void UpdateDrawable() override;

	private:
		void OnGui();
		void DrawCategoryButtons();
		void DrawDebugInfoLabels();
		
	private:
		bool initialized = false;

		Borealis::Graphics::Helpers::IBorealisRenderer& m_Renderer;
		
		ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;


		std::vector<IGUIDrawable*> runtimeGUIDrawables = {};

		std::vector<Runtime::Debug::DebugCategoryButton> categoryButtons = { };
		std::vector<Runtime::Debug::DebugInfoLabel*> debugLabels = { };

		float labelHeight = 18.f;
		static ImDrawData* p_drawData;
	};
}
