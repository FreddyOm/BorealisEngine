#pragma once
#include "../types/types.h"
#include <bitset>

namespace Borealis::Input
{
	constexpr auto MAX_GAMEPADS()
	{
		return 8;
	}

	constexpr auto MAX_INPUT_DEVICES()
	{
		return MAX_GAMEPADS() + 2;	// 1 Keyboard + 1 Mouse + Gamepads
	}

	enum class GamepadType
	{
		UNKNOWN = 0,
		XBOX_360,
		XBOX_ONE,
		DUAL_SHOCK,
		DUAL_SENSE,
	};

	enum class InputDeviceCategory
	{
		NONE,
		KEYBOARD,
		MOUSE,
		GAMEPAD,
		UNKNOWN,
	};

	struct GamepadAccelerometer
	{
		float x = 0;
		float y = 0;
		float z = 0;
	};

	/// <summary>
	/// An abstract representation of gamepad buttons for use with Playstation and XBOX Controllers.
	/// </summary>
	enum AbstractGamepadButtons
	{
		// Primary buttons
		BUTTON_NORTH		= 0x00000020,	// PS: Triangle / XBOX: Y
		BUTTON_WEST			= 0x00000010,	// PS: Square / XBOX: X
		BUTTON_SOUTH		= 0x00000004,	// PS: Cross / XBOX: A
		BUTTON_EAST			= 0x00000008,	// PS: Circle / XBOX: B

		// Shoulder buttons
		LEFT_SHOULDER		= 0x00000400,	// PS: L1 / XBOX: LB
		RIGHT_SHOULDER		= 0x00000800,	// PS: R1 / XBOX: RB
		
		// Thumbstick buttons
		LEFT_THUMBSTICK		= 0x00001000,
		RIGHT_THUMBSTICK	= 0x00002000,
		
		// DPad buttons
		DPAD_UP				= 0x00000040,
		DPAD_DOWN			= 0x00000080,
		DPAD_LEFT			= 0x00000100,
		DPAD_RIGHT			= 0x00000200,

		// Menu / Option buttons
		OPTIONS_LEFT		= 0x00000002,	// PS: Create / XBOX: Select
		OPTIONS_RIGHT		= 0x00000001,	// PS: Options / XBOX: Start

		// Misc buttons
		BUTTON_LOGO			= 0x00004000,	// PS: PS Logo / XBOX: N/A
		BUTTON_PAD			= 0x00008000,	// PS: Touchpad button / XBOX: N/A

	};

	/// <summary>
	/// An abstract representation of a gamepad's state for use with Playstation and XBOX Controllers.
	/// </summary>
	struct AbstractGamepadInputState
	{		
		// Thumbsticks
		float					LeftThumbstickX		= 0.0f;		// Range: -1.0f to 1.0f
		float					LeftThumbstickY		= 0.0f;		// Range: -1.0f to 1.0f
		float					RightThumbstickX	= 0.0f;		// Range: -1.0f to 1.0f
		float					RightThumbstickY	= 0.0f;		// Range: -1.0f to 1.0f

		// Touchpad (if supported)
		float					Touchpad1X			= 0.0f;		// PS: Touchpad point 1 X, Range: -1.0f to 1.0f
		float					Touchpad1Y			= 0.0f;		// PS: Touchpad point 1 Y, Range: -1.0f to 1.0f
		float					Touchpad2X			= 0.0f;		// PS: Touchpad point 2 X, Range: -1.0f to 1.0f
		float					Touchpad2Y			= 0.0f;		// PS: Touchpad point 2 Y, Range: -1.0f to 1.0f

		// Triggers
		float					LeftTrigger			= 0.0f;		// PS: L2 / XBOX: LT, Range: 0.0f to 1.0f
		float					RightTrigger		= 0.0f;		// PS: R2 / XBOX: RT, Range: 0.0f to 1.0f

		// Buttons
		Types::uint32			ButtonState = 0;

		Types::uint32			pad = 0;

		// Accelerometer
		GamepadAccelerometer	Accelerometer;

	};

	struct AbstractKeyboardInputState
	{
		std::bitset<256> keyStates{ 0 };
		std::bitset<256> prevKeyStates{ 0 };
	};

	enum AbstractMouseButtons
	{
		// Primary buttons
		BUTTON_LEFT				= 0x00000001,	// Mouse left button
		BUTTON_RIGHT			= 0x00000002,	// Mouse right button
		BUTTON_MIDDLE			= 0x00000004,	// Mouse wheel button

		// Extra buttons
		BUTTON_EXTRA1			= 0x00000008,	// Mouse extra button 1
		BUTTON_EXTRA2			= 0x000000010,	// Mouse extra button 2

		BUTTON_WHEEL_TILT_LEFT	= 0x00000020,	// Wheel tilt left
		BUTTON_WHEEL_TILT_RIGHT = 0x00000040,	// Wheel tilt right
	};

	/// <summary>
	/// An abstract representation of a mouse's state.
	/// </summary>
	struct AbstractMouseInputState
	{
		Types::uint32	buttonState = 0;

		// Mouse position
		float			PositionX	= 0.0f;
		float			PositionY	= 0.0f;

		// Mouse wheel
		float			WheelX		= 0.0f;
		float			WheelY		= 0.0f;
	};

}