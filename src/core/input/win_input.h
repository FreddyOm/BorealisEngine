#pragma once
#include "../../config.h"
#include "borealis_devices.h"
#include "input_device.h"
#include <vector>

#ifdef BOREALIS_WIN

// GameInput
#pragma comment(lib, "GameInput.lib")
#include <GameInput.h>

namespace Borealis::Input
{ 
	void InitGameInput();
	void DeinitGameInput();

	void UpdateInputState(std::vector<Gamepad>& gamepadInputStates,
		AbstractMouseInputState& mouseInputState, AbstractKeyboardInputState& keyboardInputState);
	void RegisterDevicesAndCallbacks() noexcept;
}

#endif