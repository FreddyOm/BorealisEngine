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

		PipelineDesc desc{};
		desc.SwapChain.WindowHandle = sandboxWindow.GetWindowHandle();

		BorealisD3D12Renderer renderer = BorealisD3D12Renderer(desc);
		renderer.InitializePipeline();

#if (defined BOREALIS_DEBUG || BOREALIS_RELWITHDEBINFO)
		//Runtime::RuntimeDebugger runtimeDebugger = Runtime::RuntimeDebugger(dynamic_cast<Helpers::IBorealisRenderer& const>(renderer));
#endif

		
		while (sandboxWindow.IsRunning())
		{
			sandboxWindow.UpdateWindow();

#if (defined BOREALIS_DEBUG || BOREALIS_RELWITHDEBINFO)
			//runtimeDebugger.Update();
#endif

			
		}

		renderer.DeinitializePipeline();
	}
	
	#endif

	return 0;
}
