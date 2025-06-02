#include "borealis_d3d12.h"
#include "../../debug/logger.h"
#include "../../memory/ref_cnt_auto_ptr.h"
#include "../pipeline_config.h"

using namespace Borealis::Types;
using namespace Borealis::Memory;

#if defined(BOREALIS_WIN)

#include <d3d12.h>
#include <dxgi.h>
#include <wrl.h>
#include "../helpers/d3d12_helpers.h"

using namespace Microsoft::WRL;

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
#include <d3d12sdklayers.h>	// Debug Layer
#endif

namespace Borealis::Graphics
{

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
	ComPtr<ID3D12Debug> gDebugController;
#endif

	ComPtr<ID3D12Device> gDevice;
	ComPtr<ID3D12CommandQueue> gCommandQueue;


	HRESULT InitializeD3D12Pipeline(const PipelineDesc& pipelineConfig)
	{
#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
		
		// Enable the debug layer.

		HRESULT hResult{};

		hResult = D3D12GetDebugInterface(IID_PPV_ARGS(&gDebugController));
		if (SUCCEEDED(hResult)) 
		{
			gDebugController->EnableDebugLayer();
		}
		else 
		{
			LogWarning("Could not enable debug layer in D3D12 backend: \n(%s)", StrFromHResult(hResult));
		}
			

#endif

		// Create the device.


		// Create factory to define how objects are created
		ComPtr<IDXGIFactory1> dxgiFactory;
		hResult = CreateDXGIFactory1(__uuidof(IDXGIFactory1), (void**)(dxgiFactory.GetAddressOf()));
		Assert(SUCCEEDED(hResult), "Failed to create the dxgi factory: \n(%s)", StrFromHResult(hResult));
		

		// Create the hardware adapter object
		ComPtr<IDXGIAdapter1> hardwareAdapter;

		for (Types::int32 adapterIndex = 0; DXGI_ERROR_NOT_FOUND !=
			dxgiFactory->EnumAdapters1(adapterIndex, &hardwareAdapter);
			++adapterIndex)
		{
			DXGI_ADAPTER_DESC1 desc;
			hResult = hardwareAdapter->GetDesc1(&desc);
			
			if (FAILED(hResult))
				continue;

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				continue;
			}

			hResult = D3D12CreateDevice(hardwareAdapter.Get(), pipelineConfig.MinimumFeatureLevel, __uuidof(ID3D12Device), nullptr);
			if (SUCCEEDED(hResult))
			{
				break;
			}
		}

		Assert(SUCCEEDED(hResult), "Failed to create the hardware adapter: \n(%s)", StrFromHResult(hResult));
		
		
		// Create device
		hResult = D3D12CreateDevice(hardwareAdapter.Get(), pipelineConfig.MinimumFeatureLevel, IID_PPV_ARGS(&gDevice));
		Assert(SUCCEEDED(hResult), "Failed to create the device: \n(%s)", StrFromHResult(hResult));
		
		
		
		// Create the command queue.
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		hResult = gDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&gCommandQueue));
		Assert(SUCCEEDED(hResult), "Failed to create the command queue: \n(%s)", StrFromHResult(hResult));


		// Create the swap chain.
		DXGI_SWAP_CHAIN_DESC swapChainDesc = {};
		swapChainDesc.BufferCount = pipelineConfig.SwapChain.BufferCount;
		swapChainDesc.BufferDesc.Width = pipelineConfig.SwapChain.BufferWidth;
		swapChainDesc.BufferDesc.Height = pipelineConfig.SwapChain.BufferHeight;
		swapChainDesc.BufferDesc.Format = pipelineConfig.SwapChain.BufferFormat;
		swapChainDesc.BufferUsage = pipelineConfig.SwapChain.BufferUsage;
		swapChainDesc.SwapEffect = pipelineConfig.SwapChain.SwapEffect;
		swapChainDesc.OutputWindow = pipelineConfig.SwapChain.WindowHandle;
		swapChainDesc.SampleDesc.Count = pipelineConfig.SwapChain.SampleCount;
		swapChainDesc.Windowed = pipelineConfig.SwapChain.Windowed;

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


	BOREALIS_API HRESULT InitializeD3D12(const PipelineDesc& pipelineConfig)
	{
		HRESULT hResult{};

		hResult = InitializeD3D12Pipeline(pipelineConfig);
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