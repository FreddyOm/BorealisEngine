#pragma once
#include <string>
#include "../types/types.h"

namespace Borealis::Core
{
	void OpenWindowInternal(std::string windowName = "Borealis Engine v. 0.1");
	void CloseWindowInternal();
	void OnWindowResizedInternal(Types::int16 width, Types::uint16 height);
	void UpdateWindowInternal();

	bool IsMinimizedInternal();
	bool IsRunningInternal();
	
	Types::uint8 GetWindowWidthInternal();
	Types::uint8 GetWindowHeightInternal();
	Types::int64 GetWindowHandleInternal();
}