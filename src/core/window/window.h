#pragma once
#include "../../config.h"
#include "../memory/ref_cnt_auto_ptr.h"
#include "../types/string_id.h"
#include "../types/types.h"

#include <string>

#if defined(BOREALIS_UNIX) || defined(BOREALIS_OSX)
#define GLFW_INCLUDE_VULKAN
#else
#define GLFW_INCLUDE_NONE
#endif

struct GLFWwindow;

namespace Borealis::Core
{
    enum class BOREALIS_API WindowMode
    {
        WINDOW,
        EXCLUSIVE_FULLSCREEN,
        FULLSCREEN
    };

    class BOREALIS_API BorealisWindow
    {
       public:
        virtual ~BorealisWindow() { }
        virtual void OpenWindow() = 0;
        virtual void OpenWindow(const Types::uint32 width, const Types::uint32 height) = 0;
        virtual void CloseWindow() = 0;

        virtual void UpdateWindow() const = 0;

        virtual bool IsMinimized() const = 0;
        virtual bool IsMaximized() const = 0;
        virtual bool IsOpen() const = 0;

        virtual Types::uint32 GetWindowWidth() const = 0;
        virtual Types::uint32 GetWindowHeight() const = 0;

        virtual Types::uint64 GetNativeWindowHandle() const = 0;
        virtual GLFWwindow* GetGLFWWindow() const = 0;

        virtual void SetWindowName(const std::string name) = 0;
        virtual Types::StringId GetWindowName() const = 0;

        virtual void SetWindowMode(const WindowMode mode) = 0;
        virtual const WindowMode GetWindowMode() const = 0;
    };

    class BOREALIS_API Window : public BorealisWindow
    {
       public:
        Window();
        Window(std::string windowName);

        ~Window();

        virtual void OpenWindow() override;
        virtual void OpenWindow(const Types::uint32 width, const Types::uint32 height) override;
        virtual void CloseWindow() override;

        virtual void UpdateWindow() const override;

        virtual bool IsMinimized() const override;
        virtual bool IsMaximized() const override;
        virtual bool IsOpen() const override;

        virtual Types::uint32 GetWindowWidth() const override;
        virtual Types::uint32 GetWindowHeight() const override;

        virtual Types::uint64 GetNativeWindowHandle() const override;
        virtual GLFWwindow* GetGLFWWindow() const override;

        virtual void SetWindowName(const std::string name) override;
        virtual Types::StringId GetWindowName() const override;

        virtual void SetWindowMode(const WindowMode mode) override;
        virtual const WindowMode GetWindowMode() const override;

       private:
        std::string m_windowName {};
        GLFWwindow* m_pWindow = nullptr;
        WindowMode m_WindowMode = WindowMode::WINDOW;
    };

    class BOREALIS_API NullWindow : public BorealisWindow
    {
        virtual void OpenWindow() { }
        virtual void OpenWindow(const Types::uint32 width, const Types::uint32 height) { }
        virtual void CloseWindow() { }

        virtual void UpdateWindow() const { }

        virtual bool IsMinimized() const { return false; }
        virtual bool IsMaximized() const { return false; }
        virtual bool IsOpen() const { return false; }

        virtual Types::uint32 GetWindowWidth() const { return 0; }
        virtual Types::uint32 GetWindowHeight() const { return 0; }

        virtual Types::uint64 GetNativeWindowHandle() const { return 0; }
        virtual GLFWwindow* GetGLFWWindow() const { return nullptr; }

        virtual void SetWindowName(const std::string name) { }
        virtual Types::StringId GetWindowName() const { return Types::String(""); }

        virtual void SetWindowMode(const WindowMode) { }
        virtual const WindowMode GetWindowMode() const { return WindowMode::WINDOW; }
    };

    class BOREALIS_API WindowLocator
    {
       public:
        static void Initialize()
        {
            Memory::MemAllocJanitor janitor(Memory::MemAllocatorContext::CORESYS);

            m_NullService = Memory::RefCntAutoPtr<NullWindow>::Allocate();
        }

        static Memory::RefCntAutoPtr<BorealisWindow>& Get() { return m_Service; }

        static void Provide(Memory::RefCntAutoPtr<BorealisWindow> service)
        {
            if(!service.IsValid())
            {
                // Revert to null service.
                m_Service = Memory::RefCntAutoPtr<NullWindow>::DynamicCastTo<BorealisWindow>(m_NullService);
            }
            else
            {
                m_Service = service;
            }
        }

        static void Reset()
        {
            if(m_Service.IsValid()) m_Service.Reset();

            if(m_NullService.IsValid()) m_NullService.Reset();
        }

       private:
        static Memory::RefCntAutoPtr<BorealisWindow> m_Service;
        static Memory::RefCntAutoPtr<NullWindow> m_NullService;
    };
}    // namespace Borealis::Core