#pragma once
#include <string>
#include "../types/string_id.h"
#include "../types/types.h"
#include "../../config.h"

#if defined(BOREALIS_UNIX) || defined(BOREALIS_OSX)
#define GLFW_INCLUDE_VULKAN
#else
#define GLFW_INCLUDE_NONE
#endif

struct GLFWwindow;

namespace Borealis::Core
{

	enum class BOREALIS_API WindowMode
	{
		WINDOW,
		EXCLUSIVE_FULLSCREEN,
		FULLSCREEN
	};

	class BOREALIS_API Window
	{
		public: 

			Window();
			Window(std::string windowName);

			~Window();

			void OpenWindow();
			void OpenWindow(const Types::uint32 width, const Types::uint32 height);
			void CloseWindow();

			void UpdateWindow() const;
			
			bool IsMinimized() const;
			bool IsMaximized() const;
			bool IsOpen() const;
			
			Types::uint32 GetWindowWidth() const;
			Types::uint32 GetWindowHeight() const;

			Types::uint64 GetNativeWindowHandle() const;
			GLFWwindow* GetGLFWWindow() const;
			
			void SetWindowName(const std::string name);
			Types::StringId GetWindowName() const;

			void SetWindowMode(const WindowMode);
			const WindowMode GetWindowMode() const;

		private:
			
			std::string m_windowName{};
			GLFWwindow* m_pWindow = nullptr;
			WindowMode m_WindowMode = WindowMode::WINDOW;
	};
}