#pragma once
#include "../../config.h"
#include "../debug/runtime-debug/runtime_debug.h"
#include "../graphics/helpers/helpers.h"
#include "../helpers/macros.h"
#include "../input/input.h"
#include "../window/window.h"

namespace Borealis::Core
{
    class BOREALIS_API Application
    {
       public:
        Application();
        Application(const char* appName);

        ~Application();

        BOREALIS_DELETE_COPY_CONSTRUCT(Application)
        BOREALIS_DELETE_MOVE_CONSTRUCT(Application)
        BOREALIS_DELETE_COPY_ASSIGN(Application)
        BOREALIS_DELETE_MOVE_ASSIGN(Application)

        bool IsRunning() const;
        void Update();

       private:
        void InitializeApp(const char* appName = "Borealis Engine");
        void DeinitializeApp();

       private:
        Memory::RefCntAutoPtr<Graphics::Helpers::IBorealisRenderer> m_Renderer;
        Memory::RefCntAutoPtr<Core::Window> m_Window;
        Memory::RefCntAutoPtr<Input::InputSystem> m_InputSystem;
        Memory::RefCntAutoPtr<Borealis::Runtime::Debug::RuntimeDebugger> m_RuntimeDebugger;
    };
}    // namespace Borealis::Core