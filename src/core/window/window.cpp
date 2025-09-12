#include "window.h"
#include "window_internal.h"

using namespace Borealis::Types;

namespace Borealis::Core
{
	bool Window::IsMinimized() const
	{
		return IsMinimizedInternal();
	}

	bool Window::IsRunning() const
	{
		return IsRunningInternal();
	}

	void Window::OpenWindow() const
	{
		OpenWindowInternal(windowName);
	}

	void Window::CloseWindow() const
	{
		CloseWindowInternal();
	}

	void Window::UpdateWindow() const
	{
		UpdateWindowInternal();
	}

	uint8 Window::GetWindowWidth() const
	{
		return GetWindowWidthInternal();
	}
	
	uint8 Window::GetWindowHeight() const
	{
		return GetWindowHeightInternal();
	}

	Types::uint64 Window::GetWindowHandle() const
	{
		return GetWindowHandleInternal();
	}
}

