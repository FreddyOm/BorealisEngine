#pragma once
#include "../../types/string_id.h"
#include "imgui/imgui.h"
#include "../../helpers/macros.h"

namespace Borealis::Runtime::Debug
{
	struct DebugInfoLabel
	{
	public:
		DebugInfoLabel(Types::StringId labelName, ImFont* pFont, bool isActive,
			ImVec2 size = ImVec2(0, 0),
			ImVec4 bg_color = ImVec4(0.5f, 0.5f, 0.5f, 0.2f),
			ImVec4 text_color = ImVec4(1, 1, 1, 0.8f))
			: isActive(isActive), labelName(labelName), m_pFont(pFont), m_Size(size), m_BgColor(bg_color), m_TextColor(text_color)
		{ }

		~DebugInfoLabel() = default;

		BOREALIS_DELETE_COPY_CONSTRUCT(DebugInfoLabel)
		BOREALIS_DELETE_MOVE_CONSTRUCT(DebugInfoLabel)
		BOREALIS_DELETE_COPY_ASSIGN(DebugInfoLabel)
		BOREALIS_DELETE_MOVE_ASSIGN(DebugInfoLabel)

		virtual void Draw()
		{
			ImGui::SameLine();
		}

	public:
		bool isActive = false;
		Types::StringId labelName = Types::String("");

	protected:
		ImFont* m_pFont = nullptr;
		ImVec2 m_Size = {};
		ImVec4 m_BgColor = {};
		ImVec4 m_TextColor = {};
	};
}