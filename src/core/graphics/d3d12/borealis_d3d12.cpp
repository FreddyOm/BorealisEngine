#include "borealis_d3d12.h"
#include "../../debug/logger.h"
#include "../../memory/ref_cnt_auto_ptr.h""

using namespace Borealis::Types;
using namespace Borealis::Memory;

#if defined(BOREALIS_WIN)

#include <d3d12.h>
#include <wrl.h>
#include "../helpers/d3d12_helpers.h"

using namespace Microsoft::WRL;

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
#include <d3d12sdklayers.h>	// Debug Layer
#endif

namespace Borealis::Graphics
{

	HRESULT InitializeD3D12Pipeline()
	{
#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
		// Enable the debug layer.
		{
			MemAllocJanitor janitor(MemAllocatorContext::RENDERING_DEBUG);
			


			// @TODO: Allocate debug memory -> Update RefCntAutoPtr<T> to work on D3D12 macros!
			RefCntAutoPtr<ID3D12Debug> gDebugController = RefCntAutoPtr<ID3D12Debug>::Allocate();

			ComPtr<ID3D12Debug> test;

			HRESULT hResult{};

			hResult = D3D12GetDebugInterface(BOREALIS_IID_PPV_ARGS(&gDebugController));
			if (SUCCEEDED(hResult))
			{
				gDebugController->EnableDebugLayer();
			}
			else
			{
				LogWarning("Couldn't enable debug layer in D3D12 backend: \n(%s)", StrFromHResult(hResult));
			}
			
		}

#endif

		// Create the device.

		// Create the command queue.

		// Create the swap chain.

		// Create a render target view(RTV) descriptor heap.

		// Create frame resources (a render target view for each frame).

		// Create a command allocator.

		return 0;
	}

	HRESULT InitializeAssets()
	{
		// Create an empty root signature.

		// Compile the shaders.

		// Create the vertex input layout.

		// Create a pipeline state object description, then create the object.

		// Create the command list.

		// Close the command list.

		// Create and load the vertex buffers.

		// Create the vertex buffer views.

		// Create a fence.

		// Create an event handle.

		// Wait for the GPU to finish.

		return 0;
	}


	BOREALIS_API HRESULT InitializeD3D12()
	{
		HRESULT hResult{};

		hResult = InitializeD3D12Pipeline();
		Assert(SUCCEEDED(hResult),
			"Failed to initialize the D3D12 rendering pipeline: \n(%s)", StrFromHResult(hResult));

		hResult = InitializeAssets();
		Assert(SUCCEEDED(hResult),
			"Failed to initialize the D3D12 assets: \n(%s)", StrFromHResult(hResult));


		return 0;
	}

	BOREALIS_API HRESULT DeinitializeD3D12()
	{
		

		return 0;
	}
}

#endif