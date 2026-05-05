#include "editor_window.h"
#include "imgui/imgui.h"


namespace Borealis::Runtime::Debug
{
	void EditorWindow::UpdateDrawable()
	{
		if (isOpen)
		{
			//ImGui::PushFont(font);
			ImGui::Begin(windowName.c_str(), &isOpen, ImGuiWindowFlags_NoCollapse);
			OnGui();
			ImGui::End();
			//ImGui::PopFont();
		}
	}

	void EditorWindow::OnGui()
	{
		ImGui::Text("Test Text");
	}

}
