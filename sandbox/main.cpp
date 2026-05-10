#include <core/window/window.h>
#include <core/graphics/graphics.h>
#include <core/debug/runtime-debug/runtime_debug.h>
#include <core/input/input.h>
//#include <core/helpers/events.h>
//#include <core/input/input.h>
//#include <core/types/string_id.h>
//#include <core/debug/logger.h>

using namespace Borealis::Core;
using namespace Borealis::Graphics;
using namespace Borealis::Runtime::Debug;
using namespace Borealis::Types;
using namespace Borealis::Input;

int main()
{
	#ifdef BOREALIS_WIN	// Currently only for windows while window.h doesn't have a UNIX implementation yet!
	
	{
		Window sandboxWindow = Window("Borealis Sandbox");
		sandboxWindow.OpenWindow();

		PipelineDesc desc{};
		desc.SwapChain.WindowHandle = sandboxWindow.GetNativeWindowHandle();
		desc.SwapChain.BufferHeight = sandboxWindow.GetWindowHeight();
		desc.SwapChain.BufferWidth = sandboxWindow.GetWindowWidth();

		BorealisD3D12Renderer renderer = BorealisD3D12Renderer(desc);
		InitD3D12LiveObjects();
		renderer.InitializePipeline();

		Borealis::Memory::RefCntAutoPtr<Borealis::Graphics::Texture> debugTex = renderer.CreateTexture(L"D:\\02_Repositories\\BorealisEngine\\out\\build\\x64-Debug\\sandbox\\resources\\textures\\xbox_debug_graphic.png");
		
		InputSystem inputSystem = InputSystem();

#if (defined BOREALIS_DEBUG || BOREALIS_RELWITHDEBINFO)

		Helpers::IBorealisRenderer& baseRend = dynamic_cast<Helpers::IBorealisRenderer&>(renderer);
		RuntimeDebugger runtimeDebugger = RuntimeDebugger(baseRend, &inputSystem, debugTex);
		runtimeDebugger.Attatch(sandboxWindow.GetGLFWWindow());
#endif

		while (sandboxWindow.IsOpen())
		{
			inputSystem.UpdateInputState();
			sandboxWindow.UpdateWindow();

#if (defined BOREALIS_DEBUG || BOREALIS_RELWITHDEBINFO)

			runtimeDebugger.UpdateDrawable();
#endif			
		}

#if (defined BOREALIS_DEBUG || BOREALIS_RELWITHDEBINFO)
		runtimeDebugger.Detatch();
#endif

		renderer.DeinitializePipeline();
	}
	
	Borealis::Graphics::ReportD3D12LiveObjects();	// The corresponding initialization is done in Graphics initialization code -> dependency on ID3D12Device!
	
	#endif

	return 0;
}
