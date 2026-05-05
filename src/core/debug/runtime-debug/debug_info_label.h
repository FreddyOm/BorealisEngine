#pragma once
//#include "../../../config.h"
#include "../../types/string_id.h"
#include "imgui/imgui.h"

namespace Borealis::Runtime::Debug
{
	struct DebugInfoLabel
	{
	public:
		DebugInfoLabel(Types::StringId labelName, ImFont* pFont, bool isActive,
			ImVec2 size = ImVec2(0, 0),
			ImVec4 bg_color = ImVec4(0.5f, 0.5f, 0.5f, 0.2f),
			ImVec4 text_color = ImVec4(1, 1, 1, 0.8f))
			: isActive(isActive), labelName(labelName), pFont(pFont), size(size), bg_color(bg_color), text_color(text_color)
		{ }

		virtual void Draw()
		{
			ImGui::SameLine();
		}

	public:
		bool isActive = false;
		Types::StringId labelName = Types::String("");

	protected:
		ImFont* pFont = nullptr;
		ImVec2 size = {};
		ImVec4 bg_color = {};
		ImVec4 text_color = {};
	};
}