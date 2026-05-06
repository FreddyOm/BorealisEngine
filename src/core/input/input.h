#pragma once
#include "../../config.h"
#include "input_device.h"
#include <set>
#include <vector>

namespace Borealis::Input
{	
    struct IInputSystemBase
    {
		IInputSystemBase() = default;
		virtual ~IInputSystemBase() = default;

        virtual void UpdateInputState() = 0;

        // Maybe use set aswell?
        virtual std::set<IInputDevice*>& GetAllDevices() = 0;
        
        virtual const Mouse* GetMouse() const = 0;
		virtual const Keyboard* GetKeyboard() const = 0;
		virtual const std::set<Gamepad*>& GetGamepads() const = 0;
    };


#ifdef BOREALIS_WIN    

    typedef Types::uint64 GameInputCallbackToken;
    enum GameInputDeviceStatus;
    struct IGameInputDevice;

    struct BOREALIS_API WinInputSystem : public IInputSystemBase
    {
        WinInputSystem();
        ~WinInputSystem() override;

		void UpdateInputState() override;

        static void OnDeviceConnected(IInputDevice& device, InputDeviceCategory category);
		static void OnDeviceDisconnected(IInputDevice& device, InputDeviceCategory category);

		std::set<IInputDevice*>& GetAllDevices() override;

		const Mouse* GetMouse() const override;
		const Keyboard* GetKeyboard() const override;
		const std::set<Gamepad*>& GetGamepads() const override;

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

        void UpdateInputState() override;

        static void OnDeviceConnected(IInputDevice& device, InputDeviceCategory category);
        static void OnDeviceDisconnected(IInputDevice& device, InputDeviceCategory category);

        std::set<IInputDevice*>& GetAllDevices() override;

        const Mouse* GetMouse() const override;
        const Keyboard* GetKeyboard() const override;
        const std::set<Gamepad*>& GetGamepads() const override;
    };

#elif BOREALIS_OSX

    //struct BOREALIS_API OsxInputSystem : public IInputSystemBase
    //{
    //    OsxInputSystem();
    //    ~OsxInputSystem() override;

    //    void UpdateInputState() override;

    //    void OnDeviceConnected(const IInputDevice& device, InputDeviceCategory category) override;
    //    void OnDeviceDisconnected(const IInputDevice& device, InputDeviceCategory category) override;

    //    std::vector<IInputDevice>& GetAllDevices() override;

    //    Mouse& GetMouse() override;
    //    Keyboard& GetKeyboard() override;
    //    std::vector<Gamepad>& GetGamepads() override;
    //};

#endif


#ifdef BOREALIS_WIN
    using InputSystem = WinInputSystem;
#elif BOREALIS_LINUX
    using InputSystem = LinuxInputSystem;
#elif BOREALIS_OSX
    using InputSystem = OsxInputSystem;
#endif

}