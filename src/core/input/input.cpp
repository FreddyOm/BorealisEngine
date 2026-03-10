#include "input.h"
#include "../debug/logger.h"
#include <bitset>

#ifdef BOREALIS_WIN

// XInput
#include <Xinput.h>
#pragma comment(lib, "XInput.lib")

#endif

// DualSense
//#include "../include/ds5w/ds5w.h"

namespace Borealis::Input
{	
	using namespace Borealis::Types;
	using namespace Borealis::Debug;


	std::bitset<256> prevKeyboardKeyStates{0};
	std::bitset<256> keyboardKeyStates{0};


	InputSystem::InputSystem()
	{ }

	InputSystem::~InputSystem()
	{ }



	/// <summary>
	/// Updates the input systems devices.
	/// </summary>
	void InputSystem::Update()
	{		
		
		// Do stuff

		// Finally, set prev keyboard state to current state
		prevKeyboardKeyStates = keyboardKeyStates;
	}

	
#ifdef BOREALIS_WIN

	LRESULT CALLBACK WinInputHandling(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		// TODO: Enable hooking into the callback for handling of input!
		switch (uMsg)
		{
			// Treat syskeys and normal keys the same for now, but we might want to differentiate them later on.
			case WM_SYSKEYUP:
			case WM_KEYUP:
			{
				// Invoke keyboard events right here
				keyboardKeyStates.set((int)wParam, false);
				break;
			}
			case WM_SYSKEYDOWN:
			case WM_KEYDOWN:
			{
				keyboardKeyStates.set((int)wParam, true);
				break;
			}
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

#endif
}