#pragma once
#include "../../config.h"
#include <Windows.h>

namespace Borealis::Input
{	
	BOREALIS_API LRESULT CALLBACK WinInputHandling(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

    BOREALIS_API class InputSystem 
    {
    public:

        InputSystem();
        ~InputSystem();

        void Update();

    private:
        
    };
}