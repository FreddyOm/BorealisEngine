#include <core/window/window.h>
#include <core/graphics/graphics.h>
#include <core/debug/runtime-debug/runtime-debug.h>

using namespace Borealis::Core;
using namespace Borealis::Graphics;
using namespace Borealis::Runtime::Debug;

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

		Helpers::IBorealisRenderer& const baseRend = dynamic_cast<Helpers::IBorealisRenderer& const>(renderer);
		RuntimeDebugger runtimeDebugger = RuntimeDebugger(baseRend);
#endif

		
		while (sandboxWindow.IsRunning())
		{
			sandboxWindow.UpdateWindow();

#if (defined BOREALIS_DEBUG || BOREALIS_RELWITHDEBINFO)
			//runtimeDebugger.Update();
#endif

			
		}

#if (defined BOREALIS_DEBUG || BOREALIS_RELWITHDEBINFO)
		runtimeDebugger.UninitializeGUI();
#endif

		renderer.DeinitializePipeline();
	}
	
	#endif

	return 0;
}
