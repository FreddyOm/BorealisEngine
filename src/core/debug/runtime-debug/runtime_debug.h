#pragma once
#include "../../../config.h"
#include "../../graphics/graphics.h"
#include "../../graphics/helpers/helpers.h"
#include "../../graphics/helpers/texture.h"
#include "../../helpers/macros.h"
#include "../../input/input.h"
#include "../../memory/ref_cnt_auto_ptr.h"
#include "../../types/string_id.h"
#include "../../window/window.h"

#ifdef BOREALIS_WIN
#include "../../graphics/d3d12/borealis_d3d12.h"
#endif

#include "debug_category_button.h"
#include "debug_info_label.h"
#include "IGUI_drawable.h"
#include "imgui/imgui.h"
#include "info_labels.h"
#include "input_debugger.h"
#include "memory_debugger.h"

#define IMGUI_DISABLE_OBSOLETE_FUNCTIONS

// TODO: Figure out how I can setup and use debug gui only in debug and relwithdebinfo builds
// #if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

namespace Borealis::Runtime::Debug
{

    struct BOREALIS_API RuntimeDebugger : protected IGUIDrawable
    {
        RuntimeDebugger() : IGUIDrawable(true)
        {
#ifdef WIN32
            Memory::MemAllocJanitor janitor(Memory::MemAllocatorContext::RENDERING_DEBUG);
            // Load textures for runtime debugger
            Memory::RefCntAutoPtr<Graphics::BorealisD3D12Renderer> renderer =
                Memory::RefCntAutoPtr<Graphics::Helpers::IBorealisRenderer>::DynamicCastTo<Graphics::BorealisD3D12Renderer>(
                    Graphics::RendererLocator::Get());
            Memory::RefCntAutoPtr<Graphics::Texture> debugTexAtlas = renderer->CreateTexture(
                L"D:\\02_Repositories\\BorealisEngine\\out\\build\\x64-Debug\\sandbox\\resources\\textures\\input-debug-tex-"
                L"atlas.png");

            // First, register all debug windows (deriving from IGUIDrawable)
            m_RuntimeGUIDrawables.push_back(Memory::RefCntAutoPtr<InputDebugger>::Allocate(
                Memory::RefCntAutoPtr<Input::IInputSystemBase>::DynamicCastTo<Input::InputSystem>(
                    Input::InputSystemLocator::Get()),
                debugTexAtlas));
            m_RuntimeGUIDrawables.push_back(Memory::RefCntAutoPtr<MemoryDebugger>::Allocate());

            // Then, register category buttons, passing the runtimeGUIDrawables for "click" events
            m_CategoryButtons = {
                Memory::RefCntAutoPtr<DebugCategoryButton>::Allocate(
                    ImVec2(100, 100), &m_RuntimeGUIDrawables, Types::String("CatA")),
                Memory::RefCntAutoPtr<DebugCategoryButton>::Allocate(
                    ImVec2(100, 100), &m_RuntimeGUIDrawables, Types::String("CatB")),
                Memory::RefCntAutoPtr<DebugCategoryButton>::Allocate(
                    ImVec2(100, 100), &m_RuntimeGUIDrawables, Types::String("CatC")),
                Memory::RefCntAutoPtr<DebugCategoryButton>::Allocate(
                    ImVec2(100, 100), &m_RuntimeGUIDrawables, Types::String("CatD")),
            };

            // Finally, create labels for better overview and customized statistics
            //  Add more individual metrics here (e.g. GPU time, vertex count, mesh count, ...)
            m_DebugLabels = {
                // new RuntimePauseLabel("Runtime Pause Label", inter_bold, ImVec2(labelHeight, labelHeight)),
                Memory::RefCntAutoPtr<FrameTimeDebugInfoLabel>::Allocate(
                    Types::String("Game Update Time"), calibri, ImVec2(280, m_LabelHeight)),
                Memory::RefCntAutoPtr<ImGuiDebugInfoLabel>::Allocate(
                    Types::String("ImGui Update Time"), calibri, &m_RuntimeFrameDuration, ImVec2(250, m_LabelHeight)),
                Memory::RefCntAutoPtr<VSyncInfoLabel>::Allocate(Types::String("VSync"), calibri, ImVec2(150, m_LabelHeight)),
                Memory::RefCntAutoPtr<WindowModeDebugInfoLabel>::Allocate(
                    Core::WindowLocator::Get(), Types::String("Window Mode"), calibri, ImVec2(250, m_LabelHeight)),
                // new PhysicsTimeDebugInfoLabel("Physics Update Time", inter_bold, ImVec2(115, labelHeight)),
                // new ConsoleDebugInfoLabel("Console Info", inter_bold, GetGUIDrawablePtrs(), ImVec2(130, labelHeight)),

                // The filter is always the last one
                Memory::RefCntAutoPtr<DebugLabelFilter>::Allocate(
                    Types::String("Filter"), calibri, &m_DebugLabels, ImVec2(m_LabelHeight, m_LabelHeight)),
            };
#endif
        }

        ~RuntimeDebugger()
        {
            m_RuntimeGUIDrawables.clear();
            m_CategoryButtons.clear();
            m_DebugLabels.clear();

            Detatch();
        }

        BOREALIS_DELETE_COPY_CONSTRUCT(RuntimeDebugger)
        BOREALIS_DELETE_MOVE_CONSTRUCT(RuntimeDebugger)
        BOREALIS_DELETE_COPY_ASSIGN(RuntimeDebugger)
        BOREALIS_DELETE_MOVE_ASSIGN(RuntimeDebugger)

       public:
        void Attatch();
        void Detatch();

        void Update() override;

       private:
        virtual void OnGui() override;
        void DrawCategoryButtons();
        void DrawDebugInfoLabels();

       private:
        ImGuiWindowFlags m_ImGuiFlags =
            ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoMove;

        std::vector<Memory::RefCntAutoPtr<IGUIDrawable>> m_RuntimeGUIDrawables = {};
        std::vector<Memory::RefCntAutoPtr<Runtime::Debug::DebugCategoryButton>> m_CategoryButtons = {};
        std::vector<Memory::RefCntAutoPtr<Runtime::Debug::DebugInfoLabel>> m_DebugLabels = {};

        bool m_Initialized = false;
        float m_RuntimeFrameDuration = 0.0f;
        float m_LabelHeight = 34.f;

        static ImDrawData* m_pDrawData;
    };
}    // namespace Borealis::Runtime::Debug