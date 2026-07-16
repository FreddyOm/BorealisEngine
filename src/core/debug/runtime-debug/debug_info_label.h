#pragma once
#include "../../types/string_id.h"
#include "imgui/imgui.h"
#include "../../helpers/macros.h"

namespace Borealis::Runtime::Debug
{
	struct DebugInfoLabel
	{
	public:
		DebugInfoLabel(const Types::StringId labelName, 
			ImFont* pFont, bool isActive,
			const ImVec2 size = ImVec2(0, 0),
			const ImVec4 bg_color = ImVec4(0.5f, 0.5f, 0.5f, 0.2f),
			const ImVec4 text_color = ImVec4(1, 1, 1, 0.8f))
			: m_IsActive(isActive), m_LabelName(labelName), 
			m_pFont(pFont), m_Size(size), m_BgColor(bg_color), 
			m_TextColor(text_color)
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
		bool m_IsActive = false;
		const Types::StringId m_LabelName = Types::String("");

	protected:
		ImFont* m_pFont = nullptr;
		const ImVec2 m_Size = {};
		const ImVec4 m_BgColor = {};
		const ImVec4 m_TextColor = {};
	};
}