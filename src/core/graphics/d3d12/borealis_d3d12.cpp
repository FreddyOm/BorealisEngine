#include "borealis_d3d12.h"
#include "../../debug/logger.h"
#include "../pipeline_config.h"
#include "../../memory/ref_cnt_auto_ptr.h"

using namespace Borealis::Types;
using namespace Borealis::Memory;

#if defined(BOREALIS_WIN)	// D3D12 only available for Windows OS

#include <d3d12.h>
#include <dxgi1_6.h>
#include <dxgiformat.h>
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
	ComPtr<IDXGISwapChain4> gSwapChain;
	
	Types::uint64 gFrameIndex = 0;


	HRESULT InitializeD3D12Pipeline(const PipelineDesc& pipelineConfig)
	{
		Assert(pipelineConfig.SwapChain.WindowHandle != 0,
			"Invalid window handle! Make sure to create and initialize the window before initializing the pipeline!");

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
		
		// Create factory to define how objects are created
		ComPtr<IDXGIFactory7> dxgiFactory;

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
		hResult = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, __uuidof(IDXGIFactory7), (void**)(dxgiFactory.GetAddressOf()));
#else
		hResult = CreateDXGIFactory2(0, __uuidof(IDXGIFactory7), (void**)(dxgiFactory.GetAddressOf()));
#endif

		Assert(SUCCEEDED(hResult), "Failed to create the dxgi factory: \n(%s)", StrFromHResult(hResult));


		// Create the hardware adapter object
		ComPtr<IDXGIAdapter4> hardwareAdapter;

		for (Types::int32 adapterIndex = 0; DXGI_ERROR_NOT_FOUND !=
			dxgiFactory->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(& hardwareAdapter));
			++adapterIndex)
		{
			DXGI_ADAPTER_DESC3 desc;
			hResult = hardwareAdapter->GetDesc3(&desc);
			
			if (FAILED(hResult))
				continue;

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the Basic Render Driver adapter.
				continue;
			}

			// Create device
			hResult = D3D12CreateDevice(hardwareAdapter.Get(), pipelineConfig.MinimumFeatureLevel, IID_PPV_ARGS(&gDevice));
			Assert(SUCCEEDED(hResult), "Failed to create the device: \n(%s)", StrFromHResult(hResult));

			if (SUCCEEDED(hResult))
			{
				break;
			}
		}

		Assert(SUCCEEDED(hResult), "Failed to create the hardware adapter: \n(%s)", StrFromHResult(hResult));


		// Create the command queue.
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		hResult = gDevice->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&gCommandQueue));
		Assert(SUCCEEDED(hResult), "Failed to create the command queue: \n(%s)", StrFromHResult(hResult));


		// Create the swap chain.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};

		// Common desc
		swapChainDesc.AlphaMode = pipelineConfig.SwapChain.AlphaMode;
		swapChainDesc.BufferCount = pipelineConfig.SwapChain.BufferCount;
		swapChainDesc.BufferUsage = pipelineConfig.SwapChain.BufferUsage;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
		swapChainDesc.Format = pipelineConfig.SwapChain.BufferFormat;
		swapChainDesc.Height = pipelineConfig.SwapChain.BufferHeight;
		swapChainDesc.SampleDesc.Count = pipelineConfig.SwapChain.SampleCount;
		swapChainDesc.SampleDesc.Quality = pipelineConfig.SwapChain.SampleQuality;
		swapChainDesc.Scaling = DXGI_SCALING_NONE;
		swapChainDesc.Stereo = false;;
		swapChainDesc.SwapEffect = pipelineConfig.SwapChain.SwapEffect;
		swapChainDesc.Width = pipelineConfig.SwapChain.BufferWidth;

		// Swap chain full screen desc
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFSDesc = {};
		swapChainFSDesc.RefreshRate = { pipelineConfig.SwapChain.TargetRefreshRate, 1 };
		swapChainFSDesc.Scaling = pipelineConfig.SwapChain.ScaleMode;
		swapChainFSDesc.ScanlineOrdering = pipelineConfig.SwapChain.ScaleOrdering;
		swapChainFSDesc.Windowed = pipelineConfig.SwapChain.Windowed;


		// DXGI_FORMAT_R10G10B10A10_UNORM -> HDR 10 bit --> Not available?? Why?

		Assert((swapChainDesc.SwapEffect == DXGI_SWAP_EFFECT_FLIP_DISCARD || swapChainDesc.SwapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL) ?
			pipelineConfig.SwapChain.BufferFormat == DXGI_FORMAT_R16G16B16A16_FLOAT
			|| pipelineConfig.SwapChain.BufferFormat == DXGI_FORMAT_B8G8R8A8_UNORM
			|| pipelineConfig.SwapChain.BufferFormat == DXGI_FORMAT_R8G8B8A8_UNORM
			|| pipelineConfig.SwapChain.BufferFormat == DXGI_FORMAT_R10G10B10A2_UINT
			: true, "For the current swap effect, swap chain buffer-format must be set to %i, %i, %i or %i. However it is set to %i instead!",
			DXGI_FORMAT_R16G16B16A16_FLOAT, 
			DXGI_FORMAT_B8G8R8A8_UNORM, 
			DXGI_FORMAT_R8G8B8A8_UNORM, 
			DXGI_FORMAT_R10G10B10A2_UINT, 
			pipelineConfig.SwapChain.BufferFormat);

		Assert((swapChainDesc.SwapEffect == DXGI_SWAP_EFFECT_FLIP_DISCARD || swapChainDesc.SwapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL) ?
			pipelineConfig.SwapChain.SampleCount == 1
			|| pipelineConfig.SwapChain.SampleQuality == 0
			: true, "For the current swap effect, sample count and quality must be set to SampleCount = 1 and SampleQuality = 0!");

		Assert((swapChainDesc.SwapEffect == DXGI_SWAP_EFFECT_FLIP_DISCARD || swapChainDesc.SwapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL) ?
			pipelineConfig.SwapChain.BufferCount >= 2
			&& pipelineConfig.SwapChain.BufferCount < 16
			: true, "For the current swap effect, the buffer count must be set between 2 and 16!");

		ComPtr<IDXGISwapChain1> swapChain;

#ifdef BOREALIS_CORE
		hResult = dxgiFactory->CreateSwapChain(gCommandQueue.Get(), pipelineConfig.SwapChain.WindowHandle, &swapChainDesc, &swapChain);
		Assert(SUCCEEDED(hResult), "Failed to create the swap chain: \n(%s)", StrFromHResult(hResult));
#else
		hResult = dxgiFactory->CreateSwapChainForHwnd(gCommandQueue.Get(), pipelineConfig.SwapChain.WindowHandle, &swapChainDesc, &swapChainFSDesc, NULL, &swapChain);
		Assert(SUCCEEDED(hResult), "Failed to create the swap chain: \n(%s)", StrFromHResult(hResult));
#endif

		hResult = swapChain.As(&gSwapChain);
		Assert(SUCCEEDED(hResult), "Failed to cast swap chain: \n(%s)", StrFromHResult(hResult));

		// Init frame index
		gFrameIndex = gSwapChain->GetCurrentBackBufferIndex(); 
		
		hResult = dxgiFactory->MakeWindowAssociation(pipelineConfig.SwapChain.WindowHandle, 0);
		Assert(SUCCEEDED(hResult), "Failed to make the window association: \n(%s)", StrFromHResult(hResult));


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