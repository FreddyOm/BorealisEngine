#include "window_internal.h"

#ifdef BOREALIS_WIN

#include "Windows.h"

#elif BOREALIS_LINUX

#elif BOREALIS_OSX

#endif

using namespace Borealis::Types;

namespace Borealis::Core
{

#pragma region common data 

	bool isMinimized = false;
	bool isRunning = false;

	uint16 windowWidth = 0;
	uint16 windowHeight = 0;

#pragma endregion common data

#pragma region common functions

	bool IsMinimizedInternal()
	{
		return isMinimized;
	}

	bool IsRunningInternal()
	{
		return isRunning;
	}

	uint8 GetWindowWidthInternal()
	{
		return windowWidth;
	}

	uint8 GetWindowHeightInternal()
	{
		return windowHeight;
	}

#pragma endregion common functions

#ifdef BOREALIS_WIN

	HWND windowHandle{};

	// Callback
	LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

	LRESULT CALLBACK WindowProcedure(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		// @TODO: Enable hooking into the callback for handling of input!

		switch (uMsg)
		{
			case WM_SIZE:
			{
				if (wParam == SIZE_MINIMIZED)
				{
					isMinimized = true;
				}
				else if (isMinimized)
				{
					isMinimized = false;
				}

				uint16 width = LOWORD(lParam);  // Macro to get the low-order word.
				uint16 height = HIWORD(lParam); // Macro to get the high-order word.

				// Update Size
				OnWindowResized(width, height);
				break;
			}
			case WM_CLOSE:
			{
				CloseWindowInternal();
				break;
			}
			case WM_QUIT:
			{
				PostQuitMessage(0);
				break;
			}
		}

		return DefWindowProc(hwnd, uMsg, wParam, lParam);
	}

	void OnWindowResized(int16 width, uint16 height)
	{
		windowWidth = width;
		windowHeight = height;
	}

	void OpenWindowInternal(std::string windowName)
	{
		WNDCLASS windowClass{};

		windowClass.lpfnWndProc = WindowProcedure;
		windowClass.hInstance = GetModuleHandle(NULL);	// Get current windows handle
		windowClass.lpszClassName = "DefaultWindowClass";
		
		RegisterClass(&windowClass);

		windowHandle = CreateWindowEx(
			0,                              // Optional window styles.
			windowClass.lpszClassName,		// Window class
			windowName.c_str(),				// Window name text
			WS_OVERLAPPEDWINDOW,            // Window style

			// Size and position
			CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT,

			NULL,					// Parent window    
			NULL,					// Menu
			windowClass.hInstance,  // Instance handle
			NULL					// Additional application data
		);

		// @TODO: Assert windowHandle != null!
		ShowWindow(windowHandle, SW_NORMAL);
		isRunning = true;
	}

	void CloseWindowInternal()
	{
		isRunning = false;
		DestroyWindow(windowHandle);
		windowHandle = {};
	}

	void UpdateWindowInternal()
	{
		MSG msg;
		while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
	}

#elif BOREALIS_LINUX

	void OpenWindowInternal(std::string windowName)
	{
		isRunning = true;
	}

	void CloseWindowInternal()
	{
		isRunning = false;
	}

	void UpdateWindowInternal()
	{

	}

#elif BOREALIS_OSX

	void OpenWindowInternal(std::string windowName)
	{
		isRunning = true;
	}

	void CloseWindowInternal()
	{
		isRunning = false;
	}

	void UpdateWindowInternal()
	{

	}

#endif
}
