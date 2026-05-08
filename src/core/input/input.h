#pragma once
#include "../../config.h"
#include "../helpers/macros.h"
#include "input_device.h"
#include <set>

#ifdef BOREALIS_WIN
typedef Borealis::Types::uint64 GameInputCallbackToken;
enum GameInputDeviceStatus;
struct IGameInputDevice;
#endif

namespace Borealis::Input
{	
    struct IInputSystemBase
    {
		IInputSystemBase() = default;
		virtual ~IInputSystemBase() = default;

        BOREALIS_DELETE_COPY_CONSTRUCT(IInputSystemBase)
        BOREALIS_DELETE_MOVE_CONSTRUCT(IInputSystemBase)
        BOREALIS_DELETE_COPY_ASSIGN(IInputSystemBase)
        BOREALIS_DELETE_MOVE_ASSIGN(IInputSystemBase)

        virtual void UpdateInputState() = 0;

        // Maybe use set aswell?
        virtual ::std::set<IInputDevice*>& GetAllDevices() = 0;
        
        virtual const Mouse* GetMouse() const = 0;
		virtual const Keyboard* GetKeyboard() const = 0;
		virtual const ::std::set<Gamepad*>& GetGamepads() const = 0;
    };


#ifdef BOREALIS_WIN

    struct BOREALIS_API WinInputSystem : public IInputSystemBase
    {
        WinInputSystem();
        ~WinInputSystem() override;

        BOREALIS_DELETE_COPY_CONSTRUCT(WinInputSystem)
        BOREALIS_DELETE_MOVE_CONSTRUCT(WinInputSystem)
        BOREALIS_DELETE_COPY_ASSIGN(WinInputSystem)
        BOREALIS_DELETE_MOVE_ASSIGN(WinInputSystem)

		void UpdateInputState() override;

        static void OnDeviceConnected(IInputDevice& device, InputDeviceCategory category);
		static void OnDeviceDisconnected(IInputDevice& device, InputDeviceCategory category);

		::std::set<IInputDevice*>& GetAllDevices() override;

		const Mouse* GetMouse() const override;
		const Keyboard* GetKeyboard() const override;
		const ::std::set<Gamepad*>& GetGamepads() const override;

    private:

        void RegisterDevicesAndCallbacks() noexcept;
        void RegisterDS5WInputDevices();
        void UpdateDS5WInputState();
        void UpdateGameInputState();
	};

#elif BOREALIS_LINUX

    struct LinuxInputSystem : public IInputSystemBase
    {
        LinuxInputSystem();
        ~LinuxInputSystem() override;

        BOREALIS_DEFAULT_COPY_CONSTRUCT(LinuxInputSystem)
        BOREALIS_DEFAULT_MOVE_CONSTRUCT(LinuxInputSystem)
        BOREALIS_DEFAULT_COPY_ASSIGN(LinuxInputSystem)
        BOREALIS_DEFAULT_MOVE_ASSIGN(LinuxInputSystem)

        void UpdateInputState() override;

        static void OnDeviceConnected(IInputDevice& device, InputDeviceCategory category);
        static void OnDeviceDisconnected(IInputDevice& device, InputDeviceCategory category);

        ::std::set<IInputDevice*>& GetAllDevices() override;

        const Mouse* GetMouse() const override;
        const Keyboard* GetKeyboard() const override;
        const ::std::set<Gamepad*>& GetGamepads() const override;
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

}

