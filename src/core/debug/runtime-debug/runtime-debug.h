#pragma once
#include "../../../config.h"
#include "../../graphics/pipeline_config.h"
//#include "debug_category_button.h"
#include "IGUIDrawable.h"
#include "../../graphics/helpers/helpers.h"

#include "imgui/imgui.h"
#include <vector>

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

namespace Borealis::Runtime::Debug
{

	struct BOREALIS_API RuntimeDebugger : protected IGUIDrawable
	{
		RuntimeDebugger(Graphics::Helpers::IBorealisRenderer& const renderer)
			: m_Renderer(renderer), IGUIDrawable(true)
		{ 
			/*categoryButtons =
			{
				DebugCategoryButton(ImVec2(40,40), GetGUIDrawablePtrs(), ICON_FK_VIDEO_CAMERA),
				DebugCategoryButton(ImVec2(40,40), GetGUIDrawablePtrs(), ICON_FK_CHECK_CIRCLE_O),
				DebugCategoryButton(ImVec2(40,40), GetGUIDrawablePtrs(), ICON_FK_KEYBOARD_O),
				DebugCategoryButton(ImVec2(40,40), GetGUIDrawablePtrs(), ICON_FK_TASKS),
			};*/

			//// Add more individual metrics here (e.g. GPU time, vertex count, mesh count, ...)
			//debugLabels =
			//{
			//	new RuntimePauseLabel("Runtime Pause Label", inter_bold, ImVec2(labelHeight, labelHeight)),
			//	new FrameTimeDebugInfoLabel("Game Update Time", inter_bold, ImVec2(130, labelHeight)),
			//	new ImGuiDebugInfoLabel("ImGui Update Time", inter_bold, pImgui_process_time_ms, ImVec2(115, labelHeight)),
			//	new PhysicsTimeDebugInfoLabel("Physics Update Time", inter_bold, ImVec2(115, labelHeight)),
			//	new ConsoleDebugInfoLabel("Console Info", inter_bold, GetGUIDrawablePtrs(), ImVec2(130, labelHeight)),

			//	// The filter is always the last one
			//	new DebugLabelFilter("Filter", inter_bold, &labels, ImVec2(20, labelHeight)),
			//};

			InitializeGUI();
		}
		
		~RuntimeDebugger()
		{
			UninitializeGUI();

			/*for (auto* label : debugLabels)
			{
				delete label;
			}*/
		}

	public:
		void InitializeGUI();
		void UninitializeGUI();

		void UpdateDrawable(ImFont* font) override;

	private:
		void OnGui();
		void DrawCategoryButtons();
		void DrawDebugInfoLabels();
		
	private:
		bool initialized = false;
		//Borealis::Graphics::PipelineDesc* const pPipelineDesc = nullptr;

		Borealis::Graphics::Helpers::IBorealisRenderer& m_Renderer;

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;

		//std::vector<DebugCategoryButton> categoryButtons = { };
		//std::vector<DebugInfoLabel*> debugLabels = { };

		float labelHeight = 18.f;
		static ImDrawData* p_drawData;
	};
}

#endif