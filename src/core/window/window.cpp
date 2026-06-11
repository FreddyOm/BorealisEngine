#include "window.h"

#ifdef BOREALIS_WIN
#define GLFW_EXPOSE_NATIVE_WIN32
#elif BOREALIS_UNIX
#define GLFW_EXPOSE_NATIVE_X11
#define GLFW_EXPOSE_NATIVE_WAYLAND
#endif

#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>

using namespace Borealis::Types;

namespace Borealis::Core
{
	/// <summary>
	/// Checks whether the window is minimized.
	/// </summary>
	/// <returns>true if the window is minimized; otherwise false.</returns>
	bool Window::IsMinimized() const
	{
		return m_pWindow ? glfwGetWindowAttrib(m_pWindow, GLFW_ICONIFIED) : false;
	}

	bool Window::IsMaximized() const
	{
		return m_pWindow? glfwGetWindowAttrib(m_pWindow, GLFW_MAXIMIZED) : false;
	}

	/// <summary>
	/// Checks whether the window is currently open (includes minimized) and running.
	/// </summary>
	/// <returns>true if the window is open; otherwise false.</returns>
	bool Window::IsOpen() const
	{
		return m_pWindow && !glfwWindowShouldClose(m_pWindow);
	}

	Window::Window()
		: m_windowName("Default Window")
	{
		if (!glfwInit())
		{
			glfwTerminate();
			LogError("Failed to initialize GLFW for window creation!");
		}
	}

	Window::Window(std::string windowName)
		: m_windowName(windowName)
	{
		if (!glfwInit())
		{
			glfwTerminate();
			LogError("Failed to initialize GLFW for window creation!");
		}
	}

	Window::~Window()
	{
		glfwDestroyWindow(m_pWindow);
		glfwTerminate();
	}

	/// <summary>
	/// Opens up the window with the default window size.
	/// </summary>
	void Window::OpenWindow()
	{
		OpenWindow(1920, 1080);
	}

	/// <summary>
	/// Opens up the window with the specified window size.
	/// </summary>
	void Window::OpenWindow(const Types::uint32 width, const Types::uint32 height)
	{

#ifdef BOREALIS_WIN
		// Set a window hint in order to specify no specific graphics API. 
		// This will make sure that D3D11/12 can be used for windows OS.
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

#elif defined(BOREALIS_UNIX) || defined(BOREALIS_OSX)
		// TODO: For UNIX and OSX we currently assume that Vulkan will be used!
#endif

		m_pWindow = glfwCreateWindow(width, height, m_windowName.c_str(), NULL, NULL);
		Assert(m_pWindow, 
			"Failed to create the GLFW window!");
	}


	/// <summary>
	/// Closes the window.
	/// </summary>
	void Window::CloseWindow()
	{
		Assert(IsOpen(), "Cannot close an uninitialized or terminated window!");

		glfwDestroyWindow(m_pWindow);
		glfwTerminate();
		m_pWindow = nullptr;
	}

	/// <summary>
	/// Updates the window. This function should be called every frame.
	/// </summary>
	void Window::UpdateWindow() const
	{
		Assert(IsOpen(), "Cannot update an uninitialized or terminated window!");
		glfwPollEvents();
	}

	/// <summary>
	/// Returns the width of the window.
	/// </summary>
	/// <returns>The window width.</returns>
	uint32 Window::GetWindowWidth() const
	{
		if (!IsOpen())
			return 0;

		GLFWmonitor* p_glfwMonitor = glfwGetPrimaryMonitor();
		auto* p_vidMode = glfwGetVideoMode(p_glfwMonitor);
		
		return p_vidMode->width;
	}
	
	/// <summary>
	/// Returns the height of the window.
	/// </summary>
	/// <returns>The window height.</returns>
	uint32 Window::GetWindowHeight() const
	{
		if (!IsOpen())
			return 0;

		GLFWmonitor* p_glfwMonitor = glfwGetPrimaryMonitor();
		auto* p_vidMode = glfwGetVideoMode(p_glfwMonitor);

		return p_vidMode->height;
	}

	/// <summary>
	/// Returns the native window handle.
	/// </summary>
	/// <returns>The window handle as a UINT64. Can be castet to HWND for Win32.</returns>
	Types::uint64 Window::GetNativeWindowHandle() const
	{
#ifdef BOREALIS_WIN
		
		return IsOpen() ? reinterpret_cast<Types::uint64>(glfwGetWin32Window(m_pWindow)) : 0;

#else	// BOREALIS_UNIX || BOREALIS_OSX
		
		LogError("WindowHandle for Vulkan API not yet implemented!");
		return 0;

#endif
	}

	GLFWwindow* Window::GetGLFWWindow() const
	{
		return m_pWindow;
	}

	/// <summary>
	/// Sets the window name. This will change the title of the window.
	/// </summary>
	/// <param name="name">The new window name.</param>
	void Window::SetWindowName(const std::string name)
	{
		Assert(IsOpen(), "Cannot set the window name on an uninitialized or terminated window!");

		m_windowName = name;
		glfwSetWindowTitle(m_pWindow, m_windowName.c_str());
	}

	/// <summary>
	/// Returns the name of the window.
	/// </summary>
	/// <returns>A string ID with the window name / id. Is only human-readable during debug modes.</returns>
	Types::StringId Window::GetWindowName() const
	{
		return String(m_windowName.c_str());
	}

	void Window::SetWindowMode(const WindowMode mode)
	{
		if (m_WindowMode == mode)
			return;

		GLFWmonitor* p_glfwMonitor = glfwGetPrimaryMonitor();
		const auto* p_vidMode = glfwGetVideoMode(p_glfwMonitor);

		switch (mode)
		{
			case WindowMode::FULLSCREEN:
			{
				glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
				glfwSetWindowMonitor(m_pWindow, p_glfwMonitor, 0, 0, p_vidMode->width, p_vidMode->height, GLFW_DONT_CARE);
				m_WindowMode = WindowMode::FULLSCREEN;
				break;
			}
			case WindowMode::EXCLUSIVE_FULLSCREEN:	// TODO: Fix me! Exclusive window mode does not work correctly yet! There are still some issues!
			{
				glfwWindowHint(GLFW_DECORATED, GLFW_FALSE);
				glfwSetWindowMonitor(m_pWindow, NULL, 0, 0, p_vidMode->width, p_vidMode->height, GLFW_DONT_CARE);
				m_WindowMode = WindowMode::EXCLUSIVE_FULLSCREEN;
				break;
			}
			default:
			case WindowMode::WINDOW:
			{
				glfwWindowHint(GLFW_DECORATED, GLFW_TRUE);
				glfwSetWindowMonitor(m_pWindow, NULL, 0, 60, 1920, 1080, GLFW_DONT_CARE);
				m_WindowMode = WindowMode::WINDOW;
				break;
			}
		}
	}
	const WindowMode Window::GetWindowMode() const
	{
		return m_WindowMode;
	}
}

