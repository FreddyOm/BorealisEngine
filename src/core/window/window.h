#include <string>
#include "../../borealis_engine.h"
#include "../types/types.h"

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

			Types::uint8 GetWindowWidth();
			Types::uint8 GetWindowHeight();

		private:
			
			std::string windowName{};
	};
}