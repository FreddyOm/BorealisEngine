#pragma once
#include <string>
#include "../types/types.h"
#include "../../borealis_engine.h"

namespace Borealis::Core
{
	class BOREALIS_API Window
	{
		public: 

			Window()
				: windowName("Default Window")
			{ }

			Window(std::string windowName)
				: windowName(windowName)
			{ }

			~Window()
			{ }

			void OpenWindow() const;
			void CloseWindow() const;
			void UpdateWindow() const;
			
			bool IsMinimized() const;
			bool IsRunning() const;

			Types::uint8 GetWindowWidth() const;
			Types::uint8 GetWindowHeight() const;

			Types::int64 GetWindowHandle() const;

		private:
			
			std::string windowName{};
	};
}