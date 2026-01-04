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
		desc.SwapChain.BufferHeight = sandboxWindow.GetWindowHeight();
		desc.SwapChain.BufferWidth = sandboxWindow.GetWindowWidth();

		BorealisD3D12Renderer renderer = BorealisD3D12Renderer(desc);
		InitD3D12LiveObjects();
		renderer.InitializePipeline();

#if (defined BOREALIS_DEBUG || BOREALIS_RELWITHDEBINFO)

		Helpers::IBorealisRenderer& baseRend = dynamic_cast<Helpers::IBorealisRenderer&>(renderer);
		RuntimeDebugger runtimeDebugger = RuntimeDebugger(baseRend);
#endif

		
		while (sandboxWindow.IsRunning())
		{
			sandboxWindow.UpdateWindow();

#if (defined BOREALIS_DEBUG || BOREALIS_RELWITHDEBINFO)

			runtimeDebugger.UpdateDrawable();
#endif			
		}

#if (defined BOREALIS_DEBUG || BOREALIS_RELWITHDEBINFO)
		runtimeDebugger.UninitializeGUI();
#endif

		renderer.DeinitializePipeline();
	}
	
	Borealis::Graphics::ReportD3D12LiveObjects();	// The corresponding initialization is done in Graphics initialization code -> dependency on ID3D12Device!
	
	#endif

	return 0;
}
