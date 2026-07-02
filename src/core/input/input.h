#pragma once
#include "../../config.h"
#include "../helpers/macros.h"
#include "../types/types.h"
#include "input_device.h"
#include "borealis_devices.h"
#include "../memory/ref_cnt_auto_ptr.h"

#include <set>

#ifdef BOREALIS_WIN
typedef Borealis::Types::uint64 GameInputCallbackToken;
enum GameInputDeviceStatus;
struct IGameInputDevice;
#endif

struct GLFWwindow;

namespace Borealis::Input
{	
    struct BOREALIS_API IInputSystemBase
    {
		IInputSystemBase() = default;
        virtual ~IInputSystemBase() = default;

        BOREALIS_DELETE_COPY_CONSTRUCT(IInputSystemBase)
        BOREALIS_DELETE_MOVE_CONSTRUCT(IInputSystemBase)
        BOREALIS_DELETE_COPY_ASSIGN(IInputSystemBase)
        BOREALIS_DELETE_MOVE_ASSIGN(IInputSystemBase)

        virtual void UpdateInputState() = 0;

        // Maybe use set aswell?
        virtual std::set<Memory::RefCntAutoPtr<IInputDevice>>& GetAllDevices() = 0;
        
        virtual const Memory::RefCntAutoPtr<Mouse> GetMouse() const = 0;
		virtual const Memory::RefCntAutoPtr<Keyboard> GetKeyboard() const = 0;
		virtual const std::set<Memory::RefCntAutoPtr<Gamepad>>& GetGamepads() const = 0;
    };

    class BOREALIS_API NullInputSystem : public IInputSystemBase
    {
    public:
        void UpdateInputState() { }

        virtual std::set<Memory::RefCntAutoPtr<IInputDevice>>& GetAllDevices() { return m_NullDevices; }
        virtual const Memory::RefCntAutoPtr<Mouse> GetMouse() const { return Memory::RefCntAutoPtr<Mouse>(); }
        virtual const Memory::RefCntAutoPtr<Keyboard> GetKeyboard() const { return Memory::RefCntAutoPtr<Keyboard>(); }
        virtual const std::set<Memory::RefCntAutoPtr<Gamepad>>& GetGamepads() const { return m_NullGamepads; }
   
    private:
        std::set<Memory::RefCntAutoPtr<IInputDevice>> m_NullDevices = {};
        std::set<Memory::RefCntAutoPtr<Gamepad>> m_NullGamepads = {};
    };


#ifdef BOREALIS_WIN

    struct BOREALIS_API WinInputSystem : public IInputSystemBase
    {
        WinInputSystem(GLFWwindow* window);
        ~WinInputSystem();

        BOREALIS_DELETE_COPY_CONSTRUCT(WinInputSystem)
        BOREALIS_DELETE_MOVE_CONSTRUCT(WinInputSystem)
        BOREALIS_DELETE_COPY_ASSIGN(WinInputSystem)
        BOREALIS_DELETE_MOVE_ASSIGN(WinInputSystem)

		void UpdateInputState();

        static void OnDeviceConnected(Memory::RefCntAutoPtr<IInputDevice> device, InputDeviceCategory category);
		static void OnDeviceDisconnected(Memory::RefCntAutoPtr<IInputDevice> device, InputDeviceCategory category);

		virtual std::set<Memory::RefCntAutoPtr<IInputDevice>>& GetAllDevices();

		virtual const Memory::RefCntAutoPtr<Mouse> GetMouse() const;
		virtual const Memory::RefCntAutoPtr<Keyboard> GetKeyboard() const;
		virtual const std::set<Memory::RefCntAutoPtr<Gamepad>>& GetGamepads() const;

    private:

        void RegisterDevicesAndCallbacks() noexcept;
        void RegisterDS5WInputDevices();
        void PollDS5WDeviceConnections();
        void UpdateDS5WInputState();
        void UpdateGameInputState();

    private:

        GLFWwindow* m_GLFWWindow = nullptr;
	};

#elif BOREALIS_LINUX

    struct LinuxInputSystem : public IInputSystemBase
    {
        LinuxInputSystem(GLFWwindow* window);
        ~LinuxInputSystem();

        BOREALIS_DELETE_COPY_CONSTRUCT(LinuxInputSystem)
        BOREALIS_DELETE_MOVE_CONSTRUCT(LinuxInputSystem)
        BOREALIS_DELETE_COPY_ASSIGN(LinuxInputSystem)
        BOREALIS_DELETE_MOVE_ASSIGN(LinuxInputSystem)

        virtual void UpdateInputState();

        static void OnDeviceConnected(Memory::RefCntAutoPtr<IInputDevice> device, InputDeviceCategory category);
        static void OnDeviceDisconnected(Memory::RefCntAutoPtr<IInputDevice> device, InputDeviceCategory category);

        virtual std::set<Memory::RefCntAutoPtr<IInputDevice>>& GetAllDevices();

        const Memory::RefCntAutoPtr<Mouse> GetMouse() const;
        const Memory::RefCntAutoPtr<Keyboard> GetKeyboard() const;
        const std::set<Memory::RefCntAutoPtr<Gamepad>>& GetGamepads() const;
    };

#elif BOREALIS_OSX

    //struct BOREALIS_API OsxInputSystem : public IInputSystemBase
    //{
    //    OsxInputSystem();
    //    ~OsxInputSystem() override;

    //    void UpdateInputState() override;

    //    static void OnDeviceConnected(IInputDevice& device, InputDeviceCategory category);
    //    static void OnDeviceDisconnected(IInputDevice& device, InputDeviceCategory category);

    //    std::set<IInputDevice*>& GetAllDevices() override;

    //    const Mouse* GetMouse() const override;
    //    const Keyboard* GetKeyboard() const override;
    //    const std::set<Gamepad*>& GetGamepads() const override;
    //};

#endif

#ifdef BOREALIS_WIN
    using InputSystem = Borealis::Input::WinInputSystem;
#elif BOREALIS_LINUX
    using InputSystem = Borealis::Input::LinuxInputSystem;
#elif BOREALIS_OSX
    using InputSystem = Borealis::Input::OsxInputSystem;
#endif

    class BOREALIS_API InputSystemLocator
    {
    public:
        static void Initialize() 
        { 
            Memory::MemAllocJanitor janitor(Memory::MemAllocatorContext::CORESYS);

            m_NullService = Memory::RefCntAutoPtr<NullInputSystem>::Allocate();
            m_Service = Memory::RefCntAutoPtr<NullInputSystem>::DynamicCastTo<IInputSystemBase>(m_NullService); }

        static Memory::RefCntAutoPtr<IInputSystemBase>& Get() { return m_Service; }

        static void Provide(Memory::RefCntAutoPtr<IInputSystemBase> service)
        {
            if (!service.IsValid())
            {
                // Revert to null service.
                m_Service = Memory::RefCntAutoPtr<NullInputSystem>::DynamicCastTo<IInputSystemBase>(m_NullService);
            }
            else
            {
                m_Service = service;
            }
        }

        static void Reset()
        {
            if (m_Service.IsValid())
                m_Service.Reset();

            if (m_NullService.IsValid())
                m_NullService.Reset();
        }

    private:
        static Memory::RefCntAutoPtr<IInputSystemBase> m_Service;
        static Memory::RefCntAutoPtr<NullInputSystem> m_NullService;
    };
}

