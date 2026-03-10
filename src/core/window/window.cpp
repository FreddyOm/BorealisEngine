#include "window.h"
#define GLFW_EXPOSE_NATIVE_WIN32
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
		LogError("Minimization status not yet implemented!");
		return false;
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
		: windowName("Default Window")
	{
		if (!glfwInit())
		{
			glfwTerminate();
			LogError("Failed to initialize GLFW for window creation!");
		}
	}

	Window::Window(std::string windowName)
		: windowName(windowName)
	{
		if (!glfwInit())
		{
			glfwTerminate();
			LogError("Failed to initialize GLFW for window creation!");
		}
	}

	Window::~Window()
	{
		glfwTerminate();
	}

	/// <summary>
	/// Opens up the window with the specified window name.
	/// </summary>
	void Window::OpenWindow()
	{

#ifdef BOREALIS_WIN
		// Set a window hint in order to specify no specific graphics API. 
		// This will make sure that D3D11/12 can be used for windows OS.
		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

#elif defined(BOREALIS_UNIX) || defined(BOREALIS_OSX)
		// TODO: For UNIX and OSX we currently assume that Vulkan will be used!
#endif

		m_pWindow = glfwCreateWindow(1280, 720, windowName.c_str(), NULL, NULL);
		Assert(m_pWindow, 
			"Failed to create the GLFW window!");
	}

	/// <summary>
	/// Closes the window.
	/// </summary>
	void Window::CloseWindow() const
	{
		glfwDestroyWindow(m_pWindow);
	}

	/// <summary>
	/// Updates the window. This function should be called every frame.
	/// </summary>
	void Window::UpdateWindow() const
	{
		glfwPollEvents();
	}

	/// <summary>
	/// Returns the width of the window.
	/// </summary>
	/// <returns>The window width.</returns>
	uint16 Window::GetWindowWidth() const
	{
		GLFWmonitor* p_glfwMonitor = glfwGetPrimaryMonitor();
		auto* p_vidMode = glfwGetVideoMode(p_glfwMonitor);
		
		return p_vidMode->width;
	}
	
	/// <summary>
	/// Returns the height of the window.
	/// </summary>
	/// <returns>The window height.</returns>
	uint16 Window::GetWindowHeight() const
	{
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
		
		return reinterpret_cast<Types::uint64>(glfwGetWin32Window(m_pWindow));

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
	void Window::SetWindowName(std::string name)
	{
		windowName = name;
		glfwSetWindowTitle(m_pWindow, windowName.c_str());
	}

	/// <summary>
	/// Returns the name of the window.
	/// </summary>
	/// <returns>A string ID with the window name / id. Is only human-readable during debug modes.</returns>
	Types::StringId Window::GetWindowName() const
	{
		return String(windowName.c_str());
	}
}

