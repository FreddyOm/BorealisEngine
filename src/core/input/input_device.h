#pragma once

#include "../../config.h"
#include "../helpers/macros.h"
#include "../types/string_id.h"
#include "borealis_devices.h"
#include "../types/types.h"

#include <cstring>

namespace Borealis::Input
{
	struct IInputDevice : public Types::IResettable
	{
		IInputDevice(const InputDeviceCategory deviceType
			,const Types::uint8* deviceId
			,const Types::StringId displayName)
			: DeviceType(deviceType)
			,DisplayName(displayName)
		{ 
			if(deviceId != nullptr)
				memcpy(DeviceID, deviceId, 32);
		}

		virtual void Reset() noexcept override
		{
#if (defined BOREALIS_DEBUG || BOREALIS_RELWITHDEBINFO)
			memset(&DeviceID[0], 0, 32);
#endif
			DisplayName = Types::String("N/A");
		}

		virtual ~IInputDevice() { }

		// Copy constructor
		IInputDevice(const IInputDevice& other)
			: DeviceType(other.DeviceType)
			, DisplayName(other.DisplayName)
		{ 
			memcpy(DeviceID, &other.DeviceID[0], 32);
		}

		// Move constructor
		IInputDevice(IInputDevice&& other) noexcept
			: DeviceType(other.DeviceType)
			, DisplayName(other.DisplayName)
		{
			memcpy(DeviceID, &other.DeviceID[0], 32);

			// Invalidate data
			other.DeviceType = InputDeviceCategory::NONE;
			other.DisplayName = Types::String("N/A");
			memset(&other.DeviceID[0], 0, 32);
		}

		// Copy assign
		IInputDevice& operator=(const IInputDevice& other)
		{
			this->DeviceType = other.DeviceType;
			this->DisplayName = other.DisplayName;
			memcpy(this->DeviceID, &other.DeviceID[0], 32);
			
			return *this;
		}

		// Move assign
		IInputDevice& operator=(IInputDevice&& other) noexcept
		{
			DeviceType = other.DeviceType;
			DisplayName = other.DisplayName;
			memcpy(DeviceID, &other.DeviceID[0], 32);


			// Invalidate data
			other.DeviceType = InputDeviceCategory::NONE;
			other.DisplayName = Types::String("N/A");
			memset(&other.DeviceID[0], 0, 32);

			return *this;
		}

		InputDeviceCategory DeviceType = InputDeviceCategory::NONE;
		Types::uint8 DeviceID[32] = { 0 };
		Types::StringId DisplayName = Types::String("N/A");
	};

	struct Gamepad : public IInputDevice
	{
		explicit Gamepad(const Types::uint8* deviceId
			,const Types::StringId displayName, GamepadType vendorType)
			: IInputDevice(InputDeviceCategory::GAMEPAD, deviceId, displayName), VendorType(vendorType)
		{ }
	
		~Gamepad() = default;

		BOREALIS_DELETE_COPY_CONSTRUCT(Gamepad)
		BOREALIS_DELETE_MOVE_CONSTRUCT(Gamepad)
		BOREALIS_DELETE_COPY_ASSIGN(Gamepad)
		BOREALIS_DELETE_MOVE_ASSIGN(Gamepad)

		virtual void Reset() noexcept override
		{

#if (defined BOREALIS_DEBUG || BOREALIS_RELWITHDEBINFO)
			memset(&DeviceID[0], 0, 32);
#endif
			DisplayName = Types::String("N/A");
			DeviceType = InputDeviceCategory::GAMEPAD;
			VendorType	= GamepadType::UNKNOWN;
			InputState	= {};
		}

	public:

		GamepadType VendorType = GamepadType::UNKNOWN;
		AbstractGamepadInputState InputState;
		float BatteryChargeLevel = 0;
	};

	struct Keyboard : public IInputDevice
	{
		Keyboard(const Types::uint8 deviceId [32]
			,const Types::StringId displayName)
			: IInputDevice(InputDeviceCategory::KEYBOARD, deviceId, displayName)
		{ }

		~Keyboard() = default;

		BOREALIS_DELETE_COPY_CONSTRUCT(Keyboard)
		BOREALIS_DELETE_MOVE_CONSTRUCT(Keyboard)
		BOREALIS_DELETE_COPY_ASSIGN(Keyboard)
		BOREALIS_DELETE_MOVE_ASSIGN(Keyboard)

	public:

		 AbstractKeyboardInputState InputState;
	};

	struct Mouse : public IInputDevice
	{
		Mouse(const Types::uint8 deviceId [32]
			,const Types::StringId displayName)
			: IInputDevice(InputDeviceCategory::MOUSE, deviceId, displayName)
		{ }

		~Mouse() = default;

		BOREALIS_DELETE_COPY_CONSTRUCT(Mouse)
		BOREALIS_DELETE_MOVE_CONSTRUCT(Mouse)
		BOREALIS_DELETE_COPY_ASSIGN(Mouse)
		BOREALIS_DELETE_MOVE_ASSIGN(Mouse)

	public:

		AbstractMouseInputState InputState;
	};
}