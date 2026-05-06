#include "input.h"
#include "../debug/logger.h"
#include "../types/string_id.h"
#include "../types/types.h"
#include "../helpers/object_pool.h"

#include <unordered_map>

// DualSense
#include <ds5w.h>

#ifdef BOREALIS_WIN

#if BOREALIS_WIN
#include <wrl.h>

#endif

namespace Borealis::Input
{	
	using namespace Borealis::Types;
	using namespace Borealis::Debug;

	// Borealis input devices
	Keyboard* g_Keyboard = nullptr;
	Mouse* g_Mouse = nullptr;
	Helpers::ObjectPool<Gamepad, MAX_GAMEPADS()> g_GamepadPool = {};

	std::set<IInputDevice*> g_AllDevices = {};

#ifdef BOREALIS_WIN
	
	// Microsoft GameInput Data
#pragma comment(lib, "GameInput.lib")
#include <GameInput.h>

#endif	// BOREALIS_WIN

	GameInputCallbackToken g_gameInputCallbackToken{};
	Microsoft::WRL::ComPtr<IGameInput> g_pGameInput = nullptr;

	IGameInputDevice* g_pWinKeyboardInternal = nullptr;
	IGameInputDevice* g_pWinMouseInternal = nullptr;
	std::unordered_map<uint64, IGameInputDevice*> g_pWinGamepadsInternal{};


	// DS5W DualSense input devices
	DS5W::DeviceEnumInfo g_DualSenseDeviceInfo[MAX_GAMEPADS()];
	std::unordered_map<Gamepad*, DS5W::DeviceContext> g_DualSenseDeviceContexts{};
	uint32 g_DualSenseDeviceCount = 0;


	const StringId GetDeviceTypeString(GameInputDeviceInfo const* deviceInfo)
	{
		switch (deviceInfo->deviceFamily)
		{
		case GameInputFamilyVirtual:
			return String("Virtual Device");
		case GameInputFamilyAggregate:
			return String("Aggregate Device");
		case GameInputFamilyXboxOne:
			return String("XBOX One");
		case GameInputFamilyXbox360:
			return String("XBOX 360");	
		case GameInputFamilyHid:
			switch (deviceInfo->supportedInput)
			{
			case GameInputKindKeyboard:
				return String("Keyboard");
			case GameInputKindMouse:
				return String("Mouse");
			default:
				return String("Unkown HID");
			}
		default:
			return String("Unknown");
		}
	}

	/// <summary>
	/// Hashes the 32 byte device id by adding each element to a common sum.
	/// </summary>
	/// <param name="deviceId">The deviceId</param>
	/// <returns>A unsigned integer that represents the sum of all byte elements.</returns>
	uint64 HashDeviceID(const BYTE deviceId [32])
	{
		uint64 hash = 0;
		
		for (uint8 i = 0; i < 32; ++i)
			hash += deviceId[i] * (i + 1);	// Progressively sum up the device id byte by byte

		return hash;
	}

	/// <summary>
	/// A callback that is called when an IGameInputDevice (dis-)connects.
	/// </summary>
	/// <param name="token"></param>
	/// <param name="context"></param>
	/// <param name="device"></param>
	/// <param name="timestamp"></param>
	/// <param name="currentStatus"></param>
	/// <param name="previousStatus"></param>
	void CALLBACK OnDeviceStatusChanged(
		_In_ GameInputCallbackToken token,
		_In_ void* context,
		_In_ IGameInputDevice* device,
		_In_ uint64 timestamp,
		_In_ GameInputDeviceStatus currentStatus,
		_In_ GameInputDeviceStatus previousStatus)
	{
		GameInputDeviceInfo const* deviceInfo = device->GetDeviceInfo();

		if (currentStatus & GameInputDeviceConnected)
		{
#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
			Log("Device %s with ID \"%i\" connected!", ValueFromStringId(GetDeviceTypeString(deviceInfo)), deviceInfo->deviceId.value);
#endif
			// Store the device reference for later use
			switch (deviceInfo->supportedInput)
			{
			case GameInputKindKeyboard:
				if (g_pWinKeyboardInternal == nullptr)
				{
					g_pWinKeyboardInternal = device;
					Keyboard* keyboard = new Keyboard(deviceInfo->deviceId.value, GetDeviceTypeString(deviceInfo));
					g_Keyboard = keyboard;

					WinInputSystem::OnDeviceConnected(*dynamic_cast<IInputDevice*>(keyboard), InputDeviceCategory::KEYBOARD);
				}
				break;
			case GameInputKindMouse:
				if (g_pWinMouseInternal == nullptr)
				{
					g_pWinMouseInternal = device;
					Mouse* mouse = new Mouse(deviceInfo->deviceId.value, GetDeviceTypeString(deviceInfo));
					g_Mouse = mouse;

					WinInputSystem::OnDeviceConnected(*dynamic_cast<IInputDevice*>(mouse), InputDeviceCategory::MOUSE);
				}
				break;
			default:
				switch (deviceInfo->deviceFamily)
				{
					case GameInputFamilyXbox360:
					case GameInputFamilyXboxOne:
						if (g_pWinGamepadsInternal.size() < MAX_GAMEPADS())
						{
							// Register the internal gameinput reference
							g_pWinGamepadsInternal.emplace(HashDeviceID(deviceInfo->deviceId.value), device);

							// Register the Borealis input device							
							Gamepad* gamepad = g_GamepadPool.Get(deviceInfo->deviceId.value, GetDeviceTypeString(deviceInfo),
								deviceInfo->deviceFamily == GameInputFamilyXbox360 ? GamepadType::XBOX_360 : GamepadType::XBOX_ONE);

							WinInputSystem::OnDeviceConnected(*dynamic_cast<IInputDevice*>(gamepad), InputDeviceCategory::GAMEPAD);
						}
					break;
					default:
						break;
				}
				Log("Unknown device type connected!");
			}
		}
		else
		{
#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
			Log("Device %s with ID \"%i\" disconnected!", ValueFromStringId(GetDeviceTypeString(deviceInfo)), deviceInfo->deviceId.value);
#endif

			switch (deviceInfo->supportedInput)
			{
			case GameInputKindKeyboard:
			{
				// Unregister the keyboard if removed
				if (HashDeviceID(deviceInfo->deviceId.value) == HashDeviceID(g_Keyboard->DeviceID))
				{
					WinInputSystem::OnDeviceDisconnected(*dynamic_cast<IInputDevice*>(g_Keyboard), InputDeviceCategory::KEYBOARD);
					
					delete g_Keyboard;
					g_Keyboard = nullptr;
				}
				break;
			}
			case GameInputKindMouse:
			{
				// Unregister the mouse if removed
				if (HashDeviceID(deviceInfo->deviceId.value) == HashDeviceID(g_Mouse->DeviceID))
				{
					WinInputSystem::OnDeviceDisconnected(*dynamic_cast<IInputDevice*>(g_Mouse), InputDeviceCategory::MOUSE);

					delete g_Mouse;
					g_Mouse = nullptr;
				}
				break;
			}
			default:
				switch (deviceInfo->deviceFamily)
				{
				case GameInputFamilyXbox360:
				case GameInputFamilyXboxOne:

					// Disconnect gamepad
					auto removedDevice = g_pWinGamepadsInternal.find(HashDeviceID(deviceInfo->deviceId.value));
					if (removedDevice == g_pWinGamepadsInternal.end())
						LogWarning("Couldn't find disconnected device in device list!");
					else
					{

						// Search for device with the given id
						const std::set<Gamepad*> gamepads = g_GamepadPool.GetActiveElements();

						for (std::set<Gamepad*>::iterator it = gamepads.begin(); it != gamepads.end(); ++it)
						{
							if ( HashDeviceID( (*it)->DeviceID ) == HashDeviceID( deviceInfo->deviceId.value ) )
							{
								Gamepad* _removedBorealisDevice = *it;

								// Invoke disconnect callback, remove gamepad from generic gamepad pool and remove the 
								// GameInput device after removing the generic gamepad. This cannot happen before!
								WinInputSystem::OnDeviceDisconnected(*(dynamic_cast<IInputDevice*>(_removedBorealisDevice)), InputDeviceCategory::GAMEPAD); 
								g_GamepadPool.Return(_removedBorealisDevice);
								g_pWinGamepadsInternal.erase(removedDevice);

								break;
							}
						}

					}
					break;
				}

				Log("Unknown device type disconnected!");
			}
		}

		// Invoke an input device changed event!
		// onGameInputDevicesChanged.Invoke(GetCurrentInputDevices());
	}

	WinInputSystem::WinInputSystem()
	{
		// Initialize the GameInput system
		Assert(GameInputCreate(g_pGameInput.GetAddressOf()) == S_OK,
			"Failed to create the GameInput instance.");

		g_pWinGamepadsInternal.reserve(MAX_GAMEPADS());

		// Register all devices and callbacks
		RegisterDevicesAndCallbacks();

		// Initialize DualSense input
		RegisterDS5WInputDevices();
	}

	WinInputSystem::~WinInputSystem()
	{
		// Release keyboard
		g_pWinKeyboardInternal->Release();
		g_pWinKeyboardInternal = nullptr;

		// Release mice
		g_pWinMouseInternal->Release();
		g_pWinMouseInternal = nullptr;

		// Release gamepads
		for (Types::uint8 idx = 0; idx < g_pWinGamepadsInternal.size(); ++idx)
		{
			g_pWinGamepadsInternal[idx]->Release();
			g_pWinGamepadsInternal[idx] = nullptr;
		}

		// Release game input
		g_pGameInput->UnregisterCallback(g_gameInputCallbackToken, 0);
		g_pGameInput.Reset();

		// Free Dual Sense device context
		for(auto& ctxt : g_DualSenseDeviceContexts)
			DS5W::freeDeviceContext(&ctxt.second);
	}

	void WinInputSystem::UpdateInputState()
	{
		// Update Dual Sense input
		UpdateDS5WInputState();
		UpdateGameInputState();
	}

	void WinInputSystem::OnDeviceConnected(IInputDevice& device, InputDeviceCategory category)
	{
		g_AllDevices.insert(&device);
	}

	void WinInputSystem::OnDeviceDisconnected(IInputDevice& device, InputDeviceCategory category)
	{
		g_AllDevices.erase(&device);
	}

	std::set<IInputDevice*>& WinInputSystem::GetAllDevices()
	{
		return g_AllDevices;
	}

	const Mouse* WinInputSystem::GetMouse() const
	{
		return g_Mouse;
	}

	const Keyboard* WinInputSystem::GetKeyboard() const
	{
		return g_Keyboard;
	}

	const std::set<Gamepad*>& WinInputSystem::GetGamepads() const
	{
		return g_GamepadPool.GetActiveElements();
	}

	void WinInputSystem::RegisterDevicesAndCallbacks() noexcept
	{
		GameInputCallbackToken token{};
		Assert(SUCCEEDED(g_pGameInput->RegisterDeviceCallback(
			nullptr,															// Don't filter to events from a specific device
			GameInputKindGamepad | GameInputKindKeyboard | GameInputKindMouse,	// Enumerate gamepads, keyboards, mice
			GameInputDeviceAnyStatus,											// Any device status
			GameInputBlockingEnumeration,										// Enumerate synchronously
			nullptr,															// No callback context parameter
			OnDeviceStatusChanged,												// Callback function
			&g_gameInputCallbackToken)) 										// Generated token
			, "Failed to register device callback.");
	}

	void WinInputSystem::RegisterDS5WInputDevices()
	{
		// TODO: Use this enumeration each frame in order to invoke an event for when a DualSense controller was connected!
		switch (DS5W::enumDevices(g_DualSenseDeviceInfo, MAX_GAMEPADS(), reinterpret_cast<unsigned int*>(&g_DualSenseDeviceCount)))
		{
			case DS5W_OK:
				break;
			case DS5W_E_INSUFFICIENT_BUFFER:
				LogError("DualSenseInfoBuffer not big enough for the amount of connected devices!");
				break;
		}

		// Reserve dual sense device ctxs to the amount of max connected devices
		g_DualSenseDeviceContexts.reserve(MAX_GAMEPADS());

		for (Types::uint8 deviceIdx = 0; deviceIdx < g_DualSenseDeviceCount; ++deviceIdx)
		{
			DS5W::DeviceContext ctxt;

			if (DS5W_FAILED(DS5W::initDeviceContext(&g_DualSenseDeviceInfo[deviceIdx], &ctxt)))
			{
				LogError("Failed to init Dual Sense device context!");
				continue;
			}
			
			Gamepad* gamepad = g_GamepadPool.Get(reinterpret_cast<BYTE*>(g_DualSenseDeviceInfo[deviceIdx]._internal.path),
				String("Sony DualSense Controller"), GamepadType::DUAL_SENSE);

			g_DualSenseDeviceContexts.emplace(gamepad, ctxt);

			WinInputSystem::OnDeviceConnected(*dynamic_cast<IInputDevice*>(gamepad), InputDeviceCategory::GAMEPAD);
		}
				
	}

	void WinInputSystem::UpdateDS5WInputState()
	{
		DS5W::DS5InputState inState;

		std::set<Gamepad*> gamepads = g_GamepadPool.GetActiveElements();

		for (std::set<Gamepad*>::iterator it = gamepads.begin(); it != gamepads.end(); ++it)
		{
			if ((*it)->VendorType != GamepadType::DUAL_SENSE)
				continue;

			if (DS5W_SUCCESS(DS5W::getDeviceInputState(&g_DualSenseDeviceContexts[*it], &inState)))
			{
				// Set values for thumbsticks
				(*it)->InputState.LeftThumbstickX = inState.leftStick.x;
				(*it)->InputState.LeftThumbstickY = inState.leftStick.y;
				(*it)->InputState.RightThumbstickX = inState.rightStick.x;
				(*it)->InputState.RightThumbstickY = inState.rightStick.y;
				
				// Set values for trigger
				(*it)->InputState.LeftTrigger = inState.leftTrigger;
				(*it)->InputState.RightTrigger = inState.rightTrigger;
				
				// Set values for touchpad
				(*it)->InputState.Touchpad1X = inState.touchPoint1.x;
				(*it)->InputState.Touchpad1Y = inState.touchPoint1.y;
				(*it)->InputState.Touchpad2X = inState.touchPoint2.x;
				(*it)->InputState.Touchpad2Y = inState.touchPoint2.y;

				// Set battery info
				(*it)->BatteryChargeLevel = static_cast<float>(inState.battery.level);
				(*it)->InputState.Accelerometer = { static_cast<float>(inState.accelerometer.x), static_cast<float>(inState.accelerometer.y), static_cast<float>(inState.accelerometer.z) };

				(*it)->InputState.ButtonState = 0;

				// Set values for buttons
				(*it)->InputState.ButtonState |=
					((inState.buttonsAndDpad & DS5W_ISTATE_BTX_CROSS) ? BUTTON_SOUTH : 0) |
					((inState.buttonsAndDpad & DS5W_ISTATE_BTX_CIRCLE) ? BUTTON_EAST : 0) |
					((inState.buttonsAndDpad & DS5W_ISTATE_BTX_SQUARE) ? BUTTON_WEST : 0) |
					((inState.buttonsAndDpad & DS5W_ISTATE_BTX_TRIANGLE) ? BUTTON_NORTH : 0);

				(*it)->InputState.ButtonState |=
					((inState.buttonsAndDpad & DS5W_ISTATE_DPAD_UP) ? DPAD_UP : 0) |
					((inState.buttonsAndDpad & DS5W_ISTATE_DPAD_DOWN) ? DPAD_DOWN : 0) |
					((inState.buttonsAndDpad & DS5W_ISTATE_DPAD_LEFT) ? DPAD_LEFT : 0) |
					((inState.buttonsAndDpad & DS5W_ISTATE_DPAD_RIGHT) ? DPAD_RIGHT : 0);

				(*it)->InputState.ButtonState |=
					((inState.buttonsA & DS5W_ISTATE_BTN_A_LEFT_STICK) ? LEFT_THUMBSTICK : 0) |
					((inState.buttonsA & DS5W_ISTATE_BTN_A_RIGHT_STICK) ? RIGHT_THUMBSTICK : 0) |
					((inState.buttonsA & DS5W_ISTATE_BTN_A_LEFT_BUMPER) ? LEFT_SHOULDER : 0) |
					((inState.buttonsA & DS5W_ISTATE_BTN_A_RIGHT_BUMPER) ? RIGHT_SHOULDER : 0);

				// Option buttons
				(*it)->InputState.ButtonState |=
					((inState.buttonsA & DS5W_ISTATE_BTN_A_MENU) ? OPTIONS_RIGHT : 0) |
					((inState.buttonsA & DS5W_ISTATE_BTN_A_SELECT) ? OPTIONS_LEFT : 0) |
					((inState.buttonsB & DS5W_ISTATE_BTN_B_PLAYSTATION_LOGO) ? BUTTON_LOGO : 0) |
					((inState.buttonsB & DS5W_ISTATE_BTN_B_PAD_BUTTON) ? BUTTON_PAD : 0);

				// Output state for rumble and trigger fx. 
				// TODO: Implement dynamic and generic way to talk to rumble and trigger fx


				// Only do if "dirty"?

				// Create output struct and zero it
				//DS5W::DS5OutputState outState;
				//ZeroMemory(&outState, sizeof(DS5W::DS5OutputState));

				// Do some stuff here

				// Send output to the controller
				//DS5W::setDeviceOutputState(&g_DualSenseDeviceContexts[*it], &outState);
			}
		}		
	}

	void WinInputSystem::UpdateGameInputState()
	{
		IGameInputReading* reading;

		if (SUCCEEDED(g_pGameInput->GetCurrentReading(GameInputKindMouse, g_pWinMouseInternal, &reading)))
		{
			if (g_pWinMouseInternal)
				reading->GetDevice(&g_pWinMouseInternal);

			GameInputMouseState state;
			reading->GetMouseState(&state);

			g_Mouse->InputState.PositionX = state.positionX;
			g_Mouse->InputState.PositionY = state.positionY;

			g_Mouse->InputState.WheelX = state.wheelX;
			g_Mouse->InputState.WheelY = state.wheelY;

			g_Mouse->InputState.buttonState = state.buttons;
		}

		if (SUCCEEDED(g_pGameInput->GetCurrentReading(GameInputKindKeyboard, g_pWinKeyboardInternal, &reading)))
		{
			if (g_pWinKeyboardInternal)
				reading->GetDevice(&g_pWinKeyboardInternal);

			GameInputKeyState state;
			reading->GetKeyState(reading->GetKeyCount(), &state);

			//g_Keyboard->InputState.keyStates.set(state.scanCode);
		}
		

		for (std::set<Gamepad*>::iterator it = g_GamepadPool.GetActiveElements().begin(); it != g_GamepadPool.GetActiveElements().end(); ++it)
		{
			// Skip non-Microsoft gamepads
			if ((*it)->VendorType == GamepadType::DUAL_SHOCK || (*it)->VendorType == GamepadType::DUAL_SENSE)
				continue;

			IGameInputDevice* currentIGameInputGamepad = g_pWinGamepadsInternal[HashDeviceID((*it)->DeviceID)];

			const GameInputDeviceInfo* pInfo = currentIGameInputGamepad->GetDeviceInfo();
			pInfo->deviceId;


			Gamepad* currentBorealisGamepad = (*it);

			if (SUCCEEDED(g_pGameInput->GetCurrentReading(GameInputKindGamepad, currentIGameInputGamepad, &reading)))
			{
				if (currentIGameInputGamepad)
					reading->GetDevice(&currentIGameInputGamepad);

				// Get input state
				GameInputGamepadState inState;
				reading->GetGamepadState(&inState);

				currentBorealisGamepad->InputState.ButtonState = inState.buttons;

				currentBorealisGamepad->InputState.LeftThumbstickX = inState.leftThumbstickX;
				currentBorealisGamepad->InputState.RightThumbstickX = inState.rightThumbstickX;
				currentBorealisGamepad->InputState.LeftThumbstickY = inState.leftThumbstickY;
				currentBorealisGamepad->InputState.RightThumbstickY = inState.rightThumbstickY;

				currentBorealisGamepad->InputState.LeftTrigger = inState.leftTrigger;
				currentBorealisGamepad->InputState.RightTrigger = inState.rightTrigger;

				// Get battery state
				GameInputBatteryState batState;
				currentIGameInputGamepad->GetBatteryState(&batState);
				currentBorealisGamepad->BatteryChargeLevel = batState.remainingCapacity;
				
				// Get motion state
				GameInputMotionState motionState;
				reading->GetMotionState(&motionState);
				currentBorealisGamepad->InputState.Accelerometer = { motionState.accelerationX, motionState.accelerationY, motionState.accelerationZ };
				
			}
		
		}
	}

#endif // BOREALIS_WIN

#ifdef BOREALIS_LINUX

	LinuxInputSystem::LinuxInputSystem()
	{
		Assert(false, "Not implemented yet!");
	}

	LinuxInputSystem::~LinuxInputSystem()
	{
		Assert(false, "Not implemented yet!");
	}

	void LinuxInputSystem::UpdateInputState()
	{
		Assert(false, "Not implemented yet!");
	}

	void LinuxInputSystem::OnDeviceConnected(IInputDevice& device, InputDeviceCategory category)
	{
		Assert(false, "Not implemented yet!");
	}

	void LinuxInputSystem::OnDeviceDisconnected(IInputDevice& device, InputDeviceCategory category)
	{
		Assert(false, "Not implemented yet!");
	}

	std::set<IInputDevice>& LinuxInputSystem::GetAllDevices()
	{
		Assert(false, "Not implemented yet!");
		return std::set<IInputDevice>();
	}

	const Mouse* LinuxInputSystem::GetMouse() const
	{
		Assert(false, "Not implemented yet!");
		return nullptr;
	}

	const Keyboard* LinuxInputSystem::GetKeyboard() const
	{
		Assert(false, "Not implemented yet!");
		return nullptr;
	}

	const std::set<Gamepad*>& LinuxInputSystem::GetGamepads() const
	{
		Assert(false, "Not implemented yet!");
		return std::set<Gamepad*>();
	}

#endif
}