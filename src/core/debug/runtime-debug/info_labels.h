#pragma once

#include "imgui/imgui.h"
#include "debug_info_label.h"
#include "../../window/window.h"
#include "../../memory/ref_cnt_auto_ptr.h"

namespace Borealis::Runtime::Debug
{
	class FrameTimeDebugInfoLabel : public DebugInfoLabel
	{
	public:
		FrameTimeDebugInfoLabel(Types::StringId labelName,
			ImFont* pFont, ImVec2 size = ImVec2(0, 0),
			ImVec4 bg_color = ImVec4(0.5f, 0.5f, 0.5f, 0.2f),
			ImVec4 text_color = ImVec4(1, 1, 1, 0.8f))
			: DebugInfoLabel(labelName, pFont, true, size, bg_color, text_color)
		{}

		~FrameTimeDebugInfoLabel() = default;

		BOREALIS_DELETE_COPY_CONSTRUCT(FrameTimeDebugInfoLabel)
		BOREALIS_DELETE_MOVE_CONSTRUCT(FrameTimeDebugInfoLabel)
		BOREALIS_DELETE_COPY_ASSIGN(FrameTimeDebugInfoLabel)
		BOREALIS_DELETE_MOVE_ASSIGN(FrameTimeDebugInfoLabel)

		void Draw() override
		{
			if (!isActive) { return; } // If label is not activated, don't show anything

			ImGui::PushFont(m_pFont);
			ImGui::PushStyleColor(ImGuiCol_Text, m_TextColor);
			ImGui::PushStyleColor(ImGuiCol_Button, m_BgColor);

			DebugInfoLabel::Draw();

			ImGui::DynamicTextButton(0, m_Size, "%.1d (%.1fms, avg %.1fms)", 0, 0, 0);
			//ImGui::DynamicTextButton("%.1d (%.1fms, avg %.1fms)", 0, size,
			//	static_cast<int>((1.0 / Time::deltaTime)), Time::deltaTime * 1000.0, Time::GetAverageFrameTime() * 1000.0);
			ImGui::SameLine();
			ImGui::TextUnformatted("|");

			ImGui::PopStyleColor(2);
			ImGui::PopFont();
		}

	private:
	};


	class WindowModeDebugInfoLabel : public DebugInfoLabel
	{
	public:
		WindowModeDebugInfoLabel(Core::Window* pWindow, Types::StringId labelName,
			ImFont* pFont, ImVec2 size = ImVec2(0, 0),
			ImVec4 bg_color = ImVec4(0.5f, 0.5f, 0.5f, 0.2f),
			ImVec4 text_color = ImVec4(1, 1, 1, 0.8f))
			: m_CurrentMode((Types::uint8) pWindow->GetWindowMode()), m_pWindow(pWindow), DebugInfoLabel(labelName, pFont, false, size, bg_color, text_color)
		{}

		~WindowModeDebugInfoLabel() = default;

		BOREALIS_DELETE_COPY_CONSTRUCT(WindowModeDebugInfoLabel)
		BOREALIS_DELETE_MOVE_CONSTRUCT(WindowModeDebugInfoLabel)
		BOREALIS_DELETE_COPY_ASSIGN(WindowModeDebugInfoLabel)
		BOREALIS_DELETE_MOVE_ASSIGN(WindowModeDebugInfoLabel)

		void Draw() override
		{
			if (!isActive) { return; } // If label is not activated, don't show anything

			ImGui::PushFont(m_pFont);
			ImGui::PushStyleColor(ImGuiCol_Text, m_TextColor);
			ImGui::PushStyleColor(ImGuiCol_Button, m_BgColor);

			DebugInfoLabel::Draw();

			if (ImGui::DynamicTextButton(0, m_Size, "Mode: %s", m_Modes[m_CurrentMode]))
			{
				m_CurrentMode = ++m_CurrentMode % 3;
				m_pWindow->SetWindowMode((Core::WindowMode)m_CurrentMode);
			}
			
			ImGui::SameLine();
			ImGui::TextUnformatted("|");

			ImGui::PopStyleColor(2);
			ImGui::PopFont();
		}

	private:

		Types::int8 m_CurrentMode = 0;
		Core::Window* m_pWindow = nullptr;
		const char* m_Modes[3] = {"Window", "Excl. Fullscreen", "Fullscreen"};
	};


	struct DebugLabelFilter : public DebugInfoLabel
	{
	public:
		DebugLabelFilter(Types::StringId labelName,
			ImFont* pFont, std::vector<Memory::RefCntAutoPtr<DebugInfoLabel>>* pLabels, ImVec2 size = ImVec2(0, 0),
			ImVec4 bg_color = ImVec4(0.5f, 0.5f, 0.5f, 0.2f),
			ImVec4 text_color = ImVec4(1, 1, 1, 0.8f))
			: DebugInfoLabel(labelName, pFont, true, size, bg_color, text_color),
			m_pLabels(pLabels)
		{}

		void Draw() override
		{
			if (!isActive) { return; } // If label is not activated, don't show anything

			ImGui::PushFont(m_pFont);
			ImGui::PushStyleColor(ImGuiCol_Text, m_TextColor);
			ImGui::PushStyleColor(ImGuiCol_Button, m_BgColor);

			DebugInfoLabel::Draw();
			if (ImGui::DynamicTextButton(0, m_Size, "+"))
				ImGui::OpenPopup("LabelFilter");
			if (ImGui::BeginPopup("LabelFilter"))
			{
				for (int i = 0; i < m_pLabels->size(); i++)
				{
					// User should not be able to (de-)activate the filter label itself
					if (m_pLabels->at(i).RawPtr() != this)
						ImGui::MenuItem(Types::ValueFromStringId(m_pLabels->at(i)->labelName), "", &m_pLabels->at(i)->isActive);	// TODO: This does only work in Debug config! Value from String is not available in runtime versions!
				}

				ImGui::Separator();

				if (ImGui::Selectable("Activate All"))
				{
					for (auto& label : *m_pLabels)
					{
						if (label.RawPtr() != this)
							label->isActive = true;
					}
				}
				if (ImGui::Selectable("Deactivate All"))
				{
					for (auto& label : *m_pLabels)
					{
						if (label.RawPtr() != this)
							label->isActive = false;
					}
				}

				ImGui::EndPopup();
			}

			ImGui::PopStyleColor(2);
			ImGui::PopFont();
		}

	private:
		std::vector<Memory::RefCntAutoPtr<DebugInfoLabel>>* m_pLabels = nullptr;

	};

}