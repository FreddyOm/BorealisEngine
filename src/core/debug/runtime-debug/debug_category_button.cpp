#include "../../types/types.h"
#include "debug_category_button.h"
#include "IGUI_drawable.h"

namespace Borealis::Runtime::Debug
{
	void DebugCategoryButton::Draw(Types::uint64 buttonIndex) const
	{
		if (pDrawables->size() <= buttonIndex)
			return;

		if (pDrawables->at(buttonIndex)->IsOpen())
		{
			ImGui::PushStyleColor(ImGuiCol_Text, normalColor);
			ImGui::PushStyleColor(ImGuiCol_Button, selectedColor);
		}
		else
		{
			ImGui::PushStyleColor(ImGuiCol_Text, selectedColor);
			ImGui::PushStyleColor(ImGuiCol_Button, normalColor);
		}

		if (ImGui::Button(std::string("Cat ").append(std::to_string(buttonIndex)).c_str(), size))
		{
			pDrawables->at(buttonIndex)->ToggleWindow();
		}

		ImGui::PopStyleColor(2);
	}

	ImVec4 DebugCategoryButton::normalColor = ImVec4(0.20f, 0.20f, 0.20f, 1.00f);
	ImVec4 DebugCategoryButton::selectedColor = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
}