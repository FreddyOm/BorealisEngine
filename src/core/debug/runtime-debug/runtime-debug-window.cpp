#include "runtime-debug-window.h"
#include "imgui/imgui.h"
#include <string>

// @TODO: Check if the update can be called collectively by using an event

namespace Borealis::Runtime::Debug
{
	/*void RuntimeDebugWindow::UpdateDrawable(ImFont* font = inter_light)
	{
		if (isOpen)
		{
			ImGui::PushFont(font);
			ImGui::Begin(std::string("CHANGE ME").c_str(), &isOpen, ImGuiWindowFlags_NoCollapse);
			OnGui();
			ImGui::End();
			ImGui::PopFont();
		}
	}*/
}
