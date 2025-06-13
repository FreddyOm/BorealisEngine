#include <core/window/window.h>

using namespace Borealis::Core;


int main()
{
	#ifdef BOREALIS_WIN	// Currently only for windows while window.h doesn't have a UNIX implementation yet!
	
	Window sandboxWindow = Window("Borealis Sandbox");
	sandboxWindow.OpenWindow();

	while (sandboxWindow.IsRunning())
	{
		sandboxWindow.UpdateWindow();
	}

	#endif

	return 0;
}
