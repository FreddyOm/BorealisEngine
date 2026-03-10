#pragma once
#include "../../../config.h"
#include "IGUIDrawable.h"
#include "../../graphics/helpers/helpers.h"
#include "imgui/imgui.h"
//#include "debug_category_button.h"


#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS

// TODO: Figure out how I can setup and use debug gui only in debug and relwithdebinfo builds
//#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

struct GLFWwindow;

namespace Borealis::Runtime::Debug
{
	struct BOREALIS_API RuntimeDebugger : protected IGUIDrawable
	{
		RuntimeDebugger(Graphics::Helpers::IBorealisRenderer& renderer)
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
		}
		
		~RuntimeDebugger()
		{
			Detatch();

			/*for (auto* label : debugLabels)
			{
				delete label;
			}*/
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

		//std::vector<DebugCategoryButton> categoryButtons = { };
		//std::vector<DebugInfoLabel*> debugLabels = { };

		float labelHeight = 18.f;
		static ImDrawData* p_drawData;
	};
}
