#include "window_internal.h"

#ifdef BOREALIS_WIN

#include "Windows.h"

#elif BOREALIS_LINUX

//#include <X11/Xlib.h>
//
//#include <X11/Xutil.h>
//#include <X11/XKBlib.h>

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

	void OnWindowResizedInternal(int16 width, uint16 height)
	{
		windowWidth = width;
		windowHeight = height;
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
				OnWindowResizedInternal(width, height);
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
		// Reset window data and destroy the window
		isRunning = false;
		DestroyWindow(windowHandle);
		windowHandle = {};

		// Reset window dimensions
		windowHeight = 0;
		windowWidth = 0;
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

	Types::int64 GetWindowHandleInternal()
	{
		return reinterpret_cast<Types::int64>(windowHandle);
	}

#elif BOREALIS_LINUX

	/*pDisplay = nullptr;
	Window window {};
	XEvent event{};*/

	void OpenWindowInternal(std::string windowName)
	{
		//if ((pDisplay = XOpenDisplay(NULL)) == nullptr)
		//{
		//	return;
		//}

		//auto screen = DefaultScreen(pDisplay);
		//auto rootWindow = RootWindow(pDisplay, screen);

		//// Create window 
		//window = XCreateSimpleWindow(pDisplay, rootWindow, 0, 0, 800, 500, 15, BlackPixel(pDisplay, screen), BlackPixel(pDisplay, screen));

		//if (window == 0)
		//{
		//	return;
		//}

		//XSetStandardProperties(pDisplay, window, windowName.c_str(), windowName.c_str(), None, NULL, 0, NULL);
		//XSelectInput(pDisplay, window, ExposureMask | StructureNotifyMask | PointerMotionMask | ButtonPressMask | ButtonReleaseMask | KeyPressMask | KeyReleaseMask);

		//XkbSetDetectableAutoRepeat(pDisplay, true, nullptr);

		//XMapWindow(pDisplay, window);

		//isRunning = true;
	}

	void CloseWindowInternal()
	{
		/*XUnmapWindow(pDisplay, window);
		XDestroyWindow(pDisplay, window);
		XCloseDisplay(pDisplay);

		isRunning = false;*/
	}

	void UpdateWindowInternal()
	{
		//Atom wmDeleteMessage = XInternAtom(pDisplay, "WM_DELETE_WINDOW", False);
		//XSetWMProtocols(pDisplay, window, &wmDeleteMessage, 1);

		//if (XNextEvent(pDisplay, &event) == 0)
		//{
		//	switch (event.type)
		//	{
		//		case ClientMessage:
		//		{
		//			if (event.xclient.data.l[0] == wmDeleteMessage)
		//			{
		//				// Close window
		//				WindowCloseEvent wc;
		//				SendWindowEvent(wc);

		//				CloseWindowInternal();
		//			}
		//			break;
		//		}
		//		case ConfigureNotify:
		//		{
		//			XConfigureEvent xce = event.xconfigure;

		//			if (xce.width != windowWidth || xce.height != windowHeight)
		//			{
		//				OnWindowResizedInternal(xce.width, xce.height);

		//				// Send Window Resize Event.
		//				WindowResizeEvent wr;
		//				wr.width = windowWidth;
		//				wr.height = windowHeight;
		//				SendWindowEvent(wr);
		//			}
		//			break;
		//		}
		//		case MotionNotify:
		//		{
		//			MouseMovedEvent mm;
		//			mm.x = event.xmotion.x;
		//			mm.y = event.xmotion.y;
		//			SendMouseEvent(mm);
		//			break;
		//		}
		//	}
		//}
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
