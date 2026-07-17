#pragma once

#include "imgui/imgui.h"
#include "debug_info_label.h"
#include "../../time/time.h"
#include "../../window/window.h"
#include "../../memory/ref_cnt_auto_ptr.h"
#include "../../graphics/graphics.h"
#include "../../math/math.h"

namespace Borealis::Runtime::Debug
{
	/// <summary>
	/// A label for showing the frame time of the application including the Dear ImGui frame.
	/// </summary>
	class FrameTimeDebugInfoLabel : public DebugInfoLabel
	{
	public:
		FrameTimeDebugInfoLabel(Types::StringId labelName,
			ImFont* pFont, const ImVec2 size = ImVec2(0, 0),
			const ImVec4 bg_color = ImVec4(0.5f, 0.5f, 0.5f, 0.2f),
			const ImVec4 text_color = ImVec4(1, 1, 1, 0.8f))
			: DebugInfoLabel(labelName, pFont, true, size, bg_color, text_color)
		{}

		~FrameTimeDebugInfoLabel() = default;

		BOREALIS_DELETE_COPY_CONSTRUCT(FrameTimeDebugInfoLabel)
		BOREALIS_DELETE_MOVE_CONSTRUCT(FrameTimeDebugInfoLabel)
		BOREALIS_DELETE_COPY_ASSIGN(FrameTimeDebugInfoLabel)
		BOREALIS_DELETE_MOVE_ASSIGN(FrameTimeDebugInfoLabel)

		void Draw() override
		{
			if (!m_IsActive) { return; } // If label is not activated, don't show anything

			ImGui::PushFont(m_pFont);
			ImGui::PushStyleColor(ImGuiCol_Text, m_TextColor);
			ImGui::PushStyleColor(ImGuiCol_Button, m_BgColor);

			DebugInfoLabel::Draw();
			
			if (ImGui::DynamicTextButton(0, m_Size, m_UseAvgFrameRate ? "avg %d (%.1fms, avg %.1fms)" : "%d (%.1fms, avg %.1fms)",
				static_cast<Types::int16>( m_UseAvgFrameRate ? Math::Clamp(Core::Time::GetAvgFrameRate(), 0, 999) : (1.0 / Core::Time::DeltaTime)),
				Core::Time::GetFrameTimeMs(), Core::Time::GetAvgFrameTimeMs()))
			{
				m_UseAvgFrameRate = !m_UseAvgFrameRate;
			}

			ImGui::SameLine();
			ImGui::TextUnformatted("|");

			ImGui::PopStyleColor(2);
			ImGui::PopFont();
		}

	private:

		bool m_UseAvgFrameRate = true;
	};

	/// <summary>
	/// A label for showing and changing the current window mode.
	/// </summary>
	class WindowModeDebugInfoLabel : public DebugInfoLabel
	{
	public:
		WindowModeDebugInfoLabel(Memory::RefCntAutoPtr<Core::BorealisWindow>& window, 
			Types::StringId labelName,
			ImFont* pFont, 
			const ImVec2 size = ImVec2(0, 0),
			const ImVec4 bg_color = ImVec4(0.5f, 0.5f, 0.5f, 0.2f),
			const ImVec4 text_color = ImVec4(1, 1, 1, 0.8f))
			: m_CurrentMode((Types::uint8) window->GetWindowMode()), m_Window(window), DebugInfoLabel(labelName, pFont, false, size, bg_color, text_color)
		{}

		~WindowModeDebugInfoLabel() = default;

		BOREALIS_DELETE_COPY_CONSTRUCT(WindowModeDebugInfoLabel)
		BOREALIS_DELETE_MOVE_CONSTRUCT(WindowModeDebugInfoLabel)
		BOREALIS_DELETE_COPY_ASSIGN(WindowModeDebugInfoLabel)
		BOREALIS_DELETE_MOVE_ASSIGN(WindowModeDebugInfoLabel)

		void Draw() override
		{
			if (!m_IsActive) { return; } // If label is not activated, don't show anything

			ImGui::PushFont(m_pFont);
			ImGui::PushStyleColor(ImGuiCol_Text, m_TextColor);
			ImGui::PushStyleColor(ImGuiCol_Button, m_BgColor);

			DebugInfoLabel::Draw();

			if (ImGui::DynamicTextButton(0, m_Size, "Mode: %s", m_Modes[m_CurrentMode]))
			{
				m_CurrentMode = ++m_CurrentMode % 3;
				m_Window->SetWindowMode((Core::WindowMode)m_CurrentMode);
			}
			
			ImGui::SameLine();
			ImGui::TextUnformatted("|");

			ImGui::PopStyleColor(2);
			ImGui::PopFont();
		}

	private:

		Types::int8 m_CurrentMode = 0;
		Memory::RefCntAutoPtr<Core::BorealisWindow> m_Window;
		const char* m_Modes[3] = {"Window", "Excl. Fullscreen", "Fullscreen"};
	};

	/// <summary>
	/// A filter for activating and deactivating debug info labels.
	/// </summary>
	struct DebugLabelFilter : public DebugInfoLabel
	{
	public:
		DebugLabelFilter(Types::StringId labelName,
			ImFont* pFont, std::vector<Memory::RefCntAutoPtr<DebugInfoLabel>>* pLabels, 
			const ImVec2 size = ImVec2(0, 0),
			const ImVec4 bg_color = ImVec4(0.5f, 0.5f, 0.5f, 0.2f),
			const ImVec4 text_color = ImVec4(1, 1, 1, 0.8f))
			: DebugInfoLabel(labelName, pFont, true, size, bg_color, text_color),
			m_pLabels(pLabels)
		{}

		~DebugLabelFilter() = default;

		BOREALIS_DELETE_COPY_CONSTRUCT(DebugLabelFilter)
		BOREALIS_DELETE_MOVE_CONSTRUCT(DebugLabelFilter)
		BOREALIS_DELETE_COPY_ASSIGN(DebugLabelFilter)
		BOREALIS_DELETE_MOVE_ASSIGN(DebugLabelFilter)

		void Draw() override
		{
			if (!m_IsActive) { return; } // If label is not activated, don't show anything

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
						ImGui::MenuItem(Types::ValueFromStringId(m_pLabels->at(i)->m_LabelName), "", &m_pLabels->at(i)->m_IsActive);	// TODO: This does only work in Debug config! Value from String is not available in runtime versions!
				}

				ImGui::Separator();

				if (ImGui::Selectable("Activate All"))
				{
					for (auto& label : *m_pLabels)
					{
						if (label.RawPtr() != this)
							label->m_IsActive = true;
					}
				}
				if (ImGui::Selectable("Deactivate All"))
				{
					for (auto& label : *m_pLabels)
					{
						if (label.RawPtr() != this)
							label->m_IsActive = false;
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

	/// <summary>
	/// Debug label for showing the frame time of the 'Dear ImGui' frame.
	/// </summary>
	struct ImGuiDebugInfoLabel : public DebugInfoLabel
	{
	public:
		ImGuiDebugInfoLabel(const Types::StringId labelName,
			ImFont* pFont, const float* pImGuiDebugFrameTime,
			const ImVec2 size = ImVec2(0, 0),
			const ImVec4 bg_color = ImVec4(0.0f, 0.5f, 0.0f, 0.2f),
			const ImVec4 text_color = ImVec4(1, 1, 1, 0.8f))
			: DebugInfoLabel(labelName, pFont, true, size, bg_color, text_color),
			m_pImGuiDebugFrameTime(pImGuiDebugFrameTime)
		{}

		~ImGuiDebugInfoLabel() = default;

		BOREALIS_DELETE_COPY_CONSTRUCT(ImGuiDebugInfoLabel)
		BOREALIS_DELETE_MOVE_CONSTRUCT(ImGuiDebugInfoLabel)
		BOREALIS_DELETE_COPY_ASSIGN(ImGuiDebugInfoLabel)
		BOREALIS_DELETE_MOVE_ASSIGN(ImGuiDebugInfoLabel)

		void Draw() override
		{
			if (!m_IsActive) { return; } // If label is not activated, don't show anything

			ImGui::PushFont(m_pFont);
			ImGui::PushStyleColor(ImGuiCol_Text, m_TextColor);
			
			// Color the label bg depending on the frame time
			const ImVec4 bgColor = *m_pImGuiDebugFrameTime > 1.0f ? ImVec4(0.5f, 0.0f, 0.0f, 0.2f) : 
				*m_pImGuiDebugFrameTime > 0.5f ? ImVec4(0.5f, 0.5f, 0.0f, 0.2f) : m_BgColor;
			ImGui::PushStyleColor(ImGuiCol_Button, bgColor);

			DebugInfoLabel::Draw();
			ImGui::DynamicTextButton(0, m_Size, "ImGui Update %.2fms", *m_pImGuiDebugFrameTime);

			ImGui::SameLine();
			ImGui::TextUnformatted("|");

			ImGui::PopStyleColor(2);
			ImGui::PopFont();
		}

	private:
		const float* m_pImGuiDebugFrameTime;
	};

	/// <summary>
	/// A label for showing and changing the VSync status.
	/// </summary>
	struct VSyncInfoLabel : public DebugInfoLabel
	{
	public:
		VSyncInfoLabel(const Types::StringId labelName,
			ImFont* pFont,
			const ImVec2 size = ImVec2(0, 0),
			const ImVec4 bg_color = ImVec4(0.0f, 0.5f, 0.0f, 0.2f),
			const ImVec4 text_color = ImVec4(1, 1, 1, 0.8f))
			: DebugInfoLabel(labelName, pFont, true, size, bg_color, text_color)
		{}

		~VSyncInfoLabel() = default;

		BOREALIS_DELETE_COPY_CONSTRUCT(VSyncInfoLabel)
		BOREALIS_DELETE_MOVE_CONSTRUCT(VSyncInfoLabel)
		BOREALIS_DELETE_COPY_ASSIGN(VSyncInfoLabel)
		BOREALIS_DELETE_MOVE_ASSIGN(VSyncInfoLabel)

		void Draw() override
		{
			if (!m_IsActive) { return; } // If label is not activated, don't show anything

			ImGui::PushFont(m_pFont);
			ImGui::PushStyleColor(ImGuiCol_Text, m_TextColor);

			// Color the label bg depending on the frame time
			const ImVec4 bgColor = !Graphics::RendererLocator::Get()->IsVsyncEnabled() ? ImVec4(0.5f, 0.0f, 0.0f, 0.2f) : m_BgColor;
			ImGui::PushStyleColor(ImGuiCol_Button, bgColor);

			DebugInfoLabel::Draw();
			if (ImGui::DynamicTextButton(0, m_Size, "VSync: %s", Graphics::RendererLocator::Get()->IsVsyncEnabled() ? "ON" : "OFF"))
			{
				Graphics::RendererLocator::Get()->SetVsyncEnabled(!Graphics::RendererLocator::Get()->IsVsyncEnabled());
			}

			ImGui::SameLine();
			ImGui::TextUnformatted("|");

			ImGui::PopStyleColor(2);
			ImGui::PopFont();
		}
	};


}