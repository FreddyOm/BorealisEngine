#include <core/window/window.h>

using namespace Borealis::Core;

#ifdef BOREALIS_WIN	// Currently only for windows while window.h doesn't have a UNIX implementation yet!

int main()
{
	Window sandboxWindow = Window("Borealis Sandbox");
	sandboxWindow.OpenWindow();

	while (sandboxWindow.IsRunning())
	{
		sandboxWindow.UpdateWindow();
	}

	return 0;
}

#endif