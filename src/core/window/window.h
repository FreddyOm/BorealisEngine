#pragma once
#include <string>
#include "../types/string_id.h"
#include "../types/types.h"

#if defined(BOREALIS_UNIX) || defined(BOREALIS_OSX)
#define GLFW_INCLUDE_VULKAN
#else
#define GLFW_INCLUDE_NONE
#endif

#include "../../thirdparty/glfw/include/GLFW/glfw3.h"
//#include <GLFW/glfw3.h>

namespace Borealis::Core
{

	class BOREALIS_API Window
	{
		public: 

			Window();
			Window(std::string windowName);

			~Window();

			void OpenWindow();
			void CloseWindow() const;
			void UpdateWindow() const;
			
			bool IsMinimized() const;
			bool IsOpen() const;
			
			Types::uint16 GetWindowWidth() const;
			Types::uint16 GetWindowHeight() const;

			Types::uint64 GetNativeWindowHandle() const;
			GLFWwindow* GetGLFWWindow() const { return m_pWindow; }
			
			void SetWindowName(std::string name);
			Types::StringId GetWindowName() const;

		private:
			
			std::string windowName{};
			GLFWwindow* m_pWindow = nullptr;
	};
}