#include "win_input.h"
#include "../debug/logger.h"
#include <algorithm>
#include "../types/string_id.h"

#ifdef BOREALIS_WIN

#include <wrl.h>

namespace Borealis::Input
{
	using namespace Borealis::Debug;
	using namespace Microsoft::WRL;

	// Microsoft GameInput Data
	ComPtr<IGameInput> m_pGameInput = nullptr;

	IGameInputDevice* m_pKeyboard;
	IGameInputDevice* m_pMouse;
	std::vector<IGameInputDevice *> m_pGamepads;

	GameInputCallbackToken callbackToken;

	/// <summary>
	/// Initializes the Game Input.
	/// </summary>
	


	// TODO: Check if I can use input event callbacks instead of polling for input in the main loop 
	// -> might be more efficient; Which devices do I even want to poll for input? Especially when having multiple mice or keyboards!

	/// <summary>
	/// Polls the game input device state.
	/// </summary>
	void UpdateInputState(std::vector<Gamepad>& gamepadInputStates,
		AbstractMouseInputState& mouseInputState, AbstractKeyboardInputState& keyboardInputState)
	{
		// Update gamepads first
		for (Types::uint8 gamepadIdx = 0; gamepadIdx < m_pGamepads.size(); ++gamepadIdx)
		{
			IGameInputReading* inputReading;

			if (SUCCEEDED(m_pGameInput->GetCurrentReading(GameInputKindGamepad, m_pGamepads[gamepadIdx], &inputReading)))
			{
				GameInputGamepadState state{};
				inputReading->GetGamepadState(&state);
				inputReading->Release();

				continue;

				// Update gamepad input state
				gamepadInputStates[gamepadIdx].InputState.buttonState = state.buttons;
				
				gamepadInputStates[gamepadIdx].InputState.LeftThumbstickX = state.leftThumbstickX;
				gamepadInputStates[gamepadIdx].InputState.LeftThumbstickY = state.leftThumbstickY;
				gamepadInputStates[gamepadIdx].InputState.RightThumbstickX = state.rightThumbstickX;
				gamepadInputStates[gamepadIdx].InputState.RightThumbstickY = state.rightThumbstickY;

				gamepadInputStates[gamepadIdx].InputState.LeftTrigger = state.leftTrigger;
				gamepadInputStates[gamepadIdx].InputState.RightTrigger = state.rightTrigger;

				// XBOX controllers don't have a touchpad, so gamepadInputStates[gamepadIdx].TouchpadX and 
				// gamepadInputStates[gamepadIdx].TouchpadY will be 0.0f
			}
		}
		
		// Update keyboard
		{
			IGameInputReading* inputReading;

			if (SUCCEEDED(m_pGameInput->GetCurrentReading(GameInputKindKeyboard, m_pKeyboard, &inputReading)))
			{
				GameInputKeyState state[1024] = {0};
				inputReading->GetKeyState(1024, &state[0]);
				inputReading->Release();

				// TODO: Do something with the keyboard state
				//Log("Key count: %u", inputReading->GetKeyCount());
			}
		}

		// Update mouse
		{
			IGameInputReading* inputReading;

			if (SUCCEEDED(m_pGameInput->GetCurrentReading(GameInputKindMouse, m_pMouse, &inputReading)))
			{
				GameInputMouseState state{};
				inputReading->GetMouseState(&state);
				inputReading->Release();

				// Update mouse input state
				//mouseInputState.buttonState = state.buttons;
				//mouseInputState.PositionX = state.positionX;
				//mouseInputState.PositionY = state.positionY;
				//mouseInputState.WheelX = state.wheelX;
				//mouseInputState.WheelY = state.wheelY;
			}
		}
	}

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

	/// <summary>
	/// Converts a GameInputDeviceInfo struct to a human-readable string for logging purposes.
	/// </summary>
	/// <param name="deviceInfo"></param>
	/// <returns></returns>
	std::string GetDeviceTypeString(GameInputDeviceInfo const* deviceInfo)
	{
		switch (deviceInfo->deviceFamily)
		{
		case GameInputFamilyVirtual:
			return "Virtual Device";
		case GameInputFamilyAggregate:
			return "Aggregate Device";
		case GameInputFamilyXboxOne:
			return "XBOX One";
		case GameInputFamilyXbox360:
			return "XBOX 360";
		case GameInputFamilyHid:
			switch (deviceInfo->supportedInput)
			{
			case GameInputKindKeyboard:
				return "Keyboard";
			case GameInputKindMouse:
				return "Mouse";
			default:
				return "Unkown HID";
			}
		case GameInputFamilyI8042:
			return "I8042";
		default:
			return "Unknown";
		}
	}

#endif

	/// <summary>
	/// Callback function for device status changes (e.g. connection, disconnection, etc.)
	/// </summary>
	/// <param name="callbackToken">A callback token for async processing.</param>
	/// <param name="context">?</param>
	/// <param name="device">The device that was enumerated.</param>
	/// <param name="timestamp">The enumeration time.</param>
	/// <param name="currentStatus">The current status of the device.</param>
	/// <param name="previousStatus">The previous status of the device.</param>
	void CALLBACK OnDeviceStatusChanged(
		_In_ GameInputCallbackToken token,
		_In_ void* context,
		_In_ IGameInputDevice* device,
		_In_ uint64_t timestamp,
		_In_ GameInputDeviceStatus currentStatus,
		_In_ GameInputDeviceStatus previousStatus)
	{
		GameInputDeviceInfo const* deviceInfo = device->GetDeviceInfo();

		if (currentStatus & GameInputDeviceConnected)
		{
#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
			// Application-specific code to handle the device connection
			Log("Device %s with ID \"%i\" connected!", GetDeviceTypeString(deviceInfo).c_str(), deviceInfo->deviceId.value);
#endif
			// Store the device reference for later use
			switch (deviceInfo->supportedInput)
			{
				case GameInputKindGamepad:
					if (m_pGamepads.size() < MAX_GAMEPADS())
					{
						m_pGamepads.push_back(device);
					}
					break;
				case GameInputKindKeyboard:
					if (m_pKeyboard == nullptr)
					{
						m_pKeyboard = device;
					}
					break;
				case GameInputKindMouse:
					if (m_pMouse == nullptr)
					{
						m_pMouse = device;
					}
					break;
				default:
					Log("Unknown device type connected!");
			}
		}
		else
		{
			// Application-specific code to handle the device disconnection
			Log("Device %s with ID \"%i\" disconnected!", GetDeviceTypeString(deviceInfo).c_str(), deviceInfo->deviceId.value);

			switch (deviceInfo->supportedInput)
			{
			case GameInputKindGamepad:
			{
				std::vector<IGameInputDevice*>::iterator it = std::find(m_pGamepads.begin(), m_pGamepads.end(), device);
				if (it == m_pGamepads.end())
					LogWarning("Couldn't find disconnected device in device list!");
				else
				{
					m_pGamepads.erase(it);
				}
				
				break;
			}
			default:
				Log("Unknown device type disconnected!");
			}
		}

		// TODO: Fix me! Before invoking the event, the individual devices have to be packed into a unified array. 
		// This is currently necessary because the devices are categorized, which might not actually be necessary after all.
		// Although performance is not too critical since these events are only expected a few times overall, this might be an easy fix!
		

		// Invoke an input device changed event!
		//onGameInputDevicesChanged.Invoke(GetCurrentInputDevices());
	}

	/// <summary>
	/// Registers a callback for device status changes and enumerates existing devices.
	/// </summary>
	void RegisterDevicesAndCallbacks() noexcept
	{
		GameInputCallbackToken token{};
		Assert(SUCCEEDED(m_pGameInput->RegisterDeviceCallback(
			nullptr,															// Don't filter to events from a specific device
			GameInputKindGamepad | GameInputKindKeyboard | GameInputKindMouse,	// Enumerate gamepads, keyboards, mice
			GameInputDeviceAnyStatus,											// Any device status
			GameInputBlockingEnumeration,										// Enumerate synchronously
			nullptr,															// No callback context parameter
			OnDeviceStatusChanged,												// Callback function
			&callbackToken)) 													// Generated token
			, "Failed to register device callback.");
	}
}

#endif