#include <core/window/window.h>
#include <core/graphics/graphics.h>

using namespace Borealis::Core;
using namespace Borealis::Graphics;

int main()
{
	#ifdef BOREALIS_WIN	// Currently only for windows while window.h doesn't have a UNIX implementation yet!
	
	{
		Window sandboxWindow = Window("Borealis Sandbox");
		sandboxWindow.OpenWindow();

		PipelineDesc config{};
		config.SwapChain.WindowHandle = reinterpret_cast<HWND>(sandboxWindow.GetWindowHandle());

		BorealisD3D12Renderer renderer = BorealisD3D12Renderer();
		renderer.InitializePipeline(config);

		while (sandboxWindow.IsRunning())
		{
			sandboxWindow.UpdateWindow();
		}

		renderer.DeinitializePipeline();
	}
	
	#endif

	return 0;
}
