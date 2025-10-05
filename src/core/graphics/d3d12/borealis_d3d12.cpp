#include "borealis_d3d12.h"
#include "../../debug/logger.h"
#include "../../memory/memory.h"
//#include "../../debug/runtime-debug/EditorWindow.h"

using namespace Borealis::Types;

#if defined(BOREALIS_WIN)	// D3D12 only available for Windows OS

using namespace Microsoft::WRL;

namespace Borealis::Graphics
{
	Borealis::Graphics::Helpers::D3D12DescriptorHeapAllocator g_RTVDescHeapAllocator{};
	Borealis::Graphics::Helpers::D3D12DescriptorHeapAllocator g_SRVDescHeapAllocator{};
	Borealis::Graphics::Helpers::D3D12DescriptorHeapAllocator g_DSVDescHeapAllocator{};

	BorealisD3D12Renderer::~BorealisD3D12Renderer()
	{
#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
		Assert(!m_isInitialized, 
			"Pipeline is not deinitialized! Make sure to call IBorealisRenderer::DeinitializePipeline() before destroying the renderer.");
#endif

		g_SRVDescHeapAllocator.Destroy();

	}

	int64 BorealisD3D12Renderer::InitializePipeline()
	{
		HRESULT hResult{};

		hResult = SetupPipeline();
		Assert(SUCCEEDED(hResult),
			"Failed to setup the D3D12 rendering pipeline: \n(%u)", hResult);

		hResult = SetupAssets();
		Assert(SUCCEEDED(hResult),
			"Failed to setup the D3D12 assets: \n(%u)", hResult);
		
#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
		{
			Memory::MemAllocJanitor janitor(Memory::MemAllocatorContext::DEBUG);
			//IBorealisRenderer::pRuntimeDebugger = Memory::Allocate<Borealis::Runtime::Debug::EditorWindow>("Test");
		}

		m_isInitialized = true;
#endif
		return hResult;
	}

	int64 BorealisD3D12Renderer::DeinitializePipeline()
	{
#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

		if (m_DXGIDebug)
		{
			m_DXGIDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}

		m_isInitialized = false;
#endif
		return 0;
	}

	ID3D12Device* const BorealisD3D12Renderer::GetDevice() const
	{
		return m_Device.Get();
	}

	ID3D12CommandQueue* const BorealisD3D12Renderer::GetCommandQueue() const
	{
		return m_CommandQueue.Get();
	}

	ID3D12GraphicsCommandList* const BorealisD3D12Renderer::GetCommandList() const
	{
		Assert(false, "Not implemented yet!");
		return nullptr;
	}

	IDXGISwapChain4* const BorealisD3D12Renderer::GetSwapChain() const
	{
		return m_SwapChain.Get();
	}

	ID3D12DescriptorHeap* const BorealisD3D12Renderer::GetDescriptorHeap() const
	{
		return m_RTV_DescriptorHeap.Get();
	}

	/// <summary>
	/// Registers a descriptor heap allocator for the specified descriptor heap type.
	/// </summary>
	/// <param name="descHeap">The native descriptor heap to take ownership of.</param>
	/// <param name="numDescriptors">The amount of descriptor "slots" that can be stored in the heap.</param>
	/// <param name="heapType">The heap type, that defines to which allocator the heap is added!</param>
	/// <param name="NodeMask">The adapter, the resource is associated  with.</param>
	HRESULT BorealisD3D12Renderer::RegisterDescriptorHeapAllocator(ComPtr<ID3D12DescriptorHeap>& const descHeap, 
		const Types::uint16 numDescriptors, const D3D12_DESCRIPTOR_HEAP_TYPE heapType, const Types::uint8 nodeMask) const
	{
		HRESULT hResult = S_OK;

		Assert(m_Device.Get() != nullptr, "Cannot register descriptor heap allocator when device is null!");
		Assert(numDescriptors > 0, "Number of descriptors must be greater than zero!");
		Assert(descHeap.Get() == nullptr, "Descriptor heap is already registered!");

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = heapType;
		descriptorHeapDesc.NumDescriptors = numDescriptors;
		descriptorHeapDesc.Flags = 
			heapType == D3D12_DESCRIPTOR_HEAP_TYPE_DSV || heapType == D3D12_DESCRIPTOR_HEAP_TYPE_RTV ? 
			D3D12_DESCRIPTOR_HEAP_FLAG_NONE : D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
		descriptorHeapDesc.NodeMask = nodeMask;

		hResult = m_Device->CreateDescriptorHeap(&descriptorHeapDesc, IID_PPV_ARGS(descHeap.GetAddressOf()));
		Assert(SUCCEEDED(hResult), "Failed to create the descriptor heap: \n(%u)", hResult);
		
		if (!SUCCEEDED(hResult))
			return hResult;

		// Store the descriptor heap in the appropriate allocator
		switch (heapType)
		{
			case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
			{
				g_RTVDescHeapAllocator.Create(m_Device.Get(), descHeap.Get());
				break;
			}
			case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
			{
				g_DSVDescHeapAllocator.Create(m_Device.Get(), descHeap.Get());
				break;
			}
			case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
			{
				g_SRVDescHeapAllocator.Create(m_Device.Get(), descHeap.Get());
				break;
			}
			case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
			{
				Assert(false, "Sampler descriptor heap allocator not implemented yet!");
				break;
			}
		}

		return hResult;
	}

	int64 BorealisD3D12Renderer::SetupPipeline()
	{
		Assert(m_PipelineDesc.SwapChain.WindowHandle != 0,
			"Invalid window handle! Make sure to create and initialize the window before initializing the pipeline!");

		HRESULT hResult{};

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

		// Enable the debug layer.
		hResult = D3D12GetDebugInterface(IID_PPV_ARGS(&m_DebugController));
		if (SUCCEEDED(hResult))
		{
			m_DebugController->EnableDebugLayer();
		}
		else
		{
			LogWarning("Could not enable debug layer in D3D12 backend: \n(us)", hResult);
		}

		hResult = DXGIGetDebugInterface1(0, IID_PPV_ARGS(&m_DXGIDebug));
		Assert(SUCCEEDED(hResult), "Failed to query DXGI debug interface: \n%u", hResult);

#endif

		// Create factory to define how objects are created
#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
		hResult = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, __uuidof(IDXGIFactory7), (void**)(m_DXGIFactory.GetAddressOf()));
#else
		hResult = CreateDXGIFactory2(0, __uuidof(IDXGIFactory7), (void**)(m_DXGIFactory.GetAddressOf()));
#endif

		Assert(SUCCEEDED(hResult), "Failed to create the dxgi factory: \n(%u)", hResult);

		// Create the hardware adapter object
		ComPtr<IDXGIAdapter4> hardwareAdapter;

		for (Types::int32 adapterIndex = 0; DXGI_ERROR_NOT_FOUND !=
			m_DXGIFactory->EnumAdapterByGpuPreference(adapterIndex, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&hardwareAdapter));
			++adapterIndex)
		{
			DXGI_ADAPTER_DESC3 desc;
			hResult = hardwareAdapter->GetDesc3(&desc);

			if (FAILED(hResult))
				continue;

			if (desc.Flags & DXGI_ADAPTER_FLAG_SOFTWARE)
			{
				// Don't select the basic render driver adapter.
				continue;
			}

			// Create device
			hResult = D3D12CreateDevice(hardwareAdapter.Get(), m_PipelineDesc.MinimumFeatureLevel, IID_PPV_ARGS(&m_Device));
			Assert(SUCCEEDED(hResult), "Failed to create the device: \n(%u)", hResult);

			if (SUCCEEDED(hResult))
			{
				break;
			}
		}

		Assert(SUCCEEDED(hResult), "Failed to create the hardware adapter: \n(%u)", hResult);

		// Create the command queue.
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;

		hResult = m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue));
		Assert(SUCCEEDED(hResult), "Failed to create the command queue: \n(%u)", hResult);


		// Create the swap chain.
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};

		// Common desc
		swapChainDesc.AlphaMode = m_PipelineDesc.SwapChain.AlphaMode;
		swapChainDesc.BufferCount = m_PipelineDesc.SwapChain.BufferCount;
		swapChainDesc.BufferUsage = m_PipelineDesc.SwapChain.BufferUsage;
		swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
		swapChainDesc.Format = m_PipelineDesc.SwapChain.BufferFormat;
		swapChainDesc.Height = m_PipelineDesc.SwapChain.BufferHeight;
		swapChainDesc.SampleDesc.Count = m_PipelineDesc.SwapChain.SampleCount;
		swapChainDesc.SampleDesc.Quality = m_PipelineDesc.SwapChain.SampleQuality;
		swapChainDesc.Scaling = DXGI_SCALING_NONE;
		swapChainDesc.Stereo = false;;
		swapChainDesc.SwapEffect = m_PipelineDesc.SwapChain.SwapEffect;
		swapChainDesc.Width = m_PipelineDesc.SwapChain.BufferWidth;

		// Swap chain full screen desc
		DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFSDesc = {};
		swapChainFSDesc.RefreshRate = { m_PipelineDesc.SwapChain.TargetRefreshRate, 1 };
		swapChainFSDesc.Scaling = m_PipelineDesc.SwapChain.ScaleMode;
		swapChainFSDesc.ScanlineOrdering = m_PipelineDesc.SwapChain.ScaleOrdering;
		swapChainFSDesc.Windowed = m_PipelineDesc.SwapChain.Windowed;


		// DXGI_FORMAT_R10G10B10A10_UNORM -> HDR 10 bit --> Not available?? Why?

		Assert((swapChainDesc.SwapEffect == DXGI_SWAP_EFFECT_FLIP_DISCARD || swapChainDesc.SwapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL) ?
			m_PipelineDesc.SwapChain.BufferFormat == DXGI_FORMAT_R16G16B16A16_FLOAT
			|| m_PipelineDesc.SwapChain.BufferFormat == DXGI_FORMAT_B8G8R8A8_UNORM
			|| m_PipelineDesc.SwapChain.BufferFormat == DXGI_FORMAT_R8G8B8A8_UNORM
			|| m_PipelineDesc.SwapChain.BufferFormat == DXGI_FORMAT_R10G10B10A2_UINT
			: true, "For the current swap effect, swap chain buffer-format must be set to %i, %i, %i or %i. However it is set to %i instead!",
			DXGI_FORMAT_R16G16B16A16_FLOAT,
			DXGI_FORMAT_B8G8R8A8_UNORM,
			DXGI_FORMAT_R8G8B8A8_UNORM,
			DXGI_FORMAT_R10G10B10A2_UINT,
			m_PipelineDesc.SwapChain.BufferFormat);

		Assert((swapChainDesc.SwapEffect == DXGI_SWAP_EFFECT_FLIP_DISCARD || swapChainDesc.SwapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL) ?
			m_PipelineDesc.SwapChain.SampleCount == 1
			|| m_PipelineDesc.SwapChain.SampleQuality == 0
			: true, "For the current swap effect, sample count and quality must be set to SampleCount = 1 and SampleQuality = 0!");

		Assert((swapChainDesc.SwapEffect == DXGI_SWAP_EFFECT_FLIP_DISCARD || swapChainDesc.SwapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL) ?
			m_PipelineDesc.SwapChain.BufferCount >= 2
			&& m_PipelineDesc.SwapChain.BufferCount < 16
			: true, "For the current swap effect, the buffer count must be set between 2 and 16!");

		ComPtr<IDXGISwapChain1> swapChain;

#ifdef BOREALIS_CORE
		hResult = dxgiFactory->CreateSwapChain(gCommandQueue.Get(), (HWND) m_PipelineConfiguration.SwapChain.WindowHandle, &swapChainDesc, &swapChain);
		Assert(SUCCEEDED(hResult), "Failed to create the swap chain: \n(%u)", hResult);
#else
		hResult = m_DXGIFactory->CreateSwapChainForHwnd(m_CommandQueue.Get(), (HWND) m_PipelineDesc.SwapChain.WindowHandle, &swapChainDesc, &swapChainFSDesc, NULL, &swapChain);
		Assert(SUCCEEDED(hResult), "Failed to create the swap chain: \n(%u)", hResult);
#endif

		hResult = swapChain.As(&m_SwapChain);
		Assert(SUCCEEDED(hResult), "Failed to cast swap chain: \n(%u)", hResult);

		// Init frame index
		m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

		hResult = m_DXGIFactory->MakeWindowAssociation((HWND) m_PipelineDesc.SwapChain.WindowHandle, 0);
		Assert(SUCCEEDED(hResult), "Failed to make the window association: \n(%u)", hResult);

		// TODO: Create a function to allocate descriptor heaps and create heap allocators 
		// Input should be the number of descriptors and potentially the type, as well as the variable to save it into!
		// Make sure to accomodate for specific RTV functionality like storing RTVDescriptorHandles seperately!

		// Create the render target view(RTV) descriptor heap.
		{

			RegisterDescriptorHeapAllocator(m_RTV_DescriptorHeap, 4, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

			//D3D12_DESCRIPTOR_HEAP_DESC rtvHeapDesc = {};
			//rtvHeapDesc.NumDescriptors = m_PipelineDesc.SwapChain.BufferCount;
			//rtvHeapDesc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
			//rtvHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
			//rtvHeapDesc.NodeMask = 0;
			//
			//hResult = m_Device->CreateDescriptorHeap(&rtvHeapDesc, IID_PPV_ARGS(&m_RTV_DescriptorHeap));
			//Assert(SUCCEEDED(hResult), "Failed to create the RTV descriptor heap: \n(%u)", hResult);

			//Types::uint64 rtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			//D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTV_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

			//// Init descriptor handles for render targets
			//m_RTV_DescriptorHandles.resize(m_PipelineDesc.SwapChain.BufferCount);

			//for (Types::uint64 bufferIdx = 0; bufferIdx < m_PipelineDesc.SwapChain.BufferCount; ++bufferIdx)
			//{
			//	m_RTV_DescriptorHandles[bufferIdx] = rtvHandle;
			//	rtvHandle.ptr += rtvDescriptorSize;
			//}
		}
		
		RegisterDescriptorHeapAllocator(m_SRV_DescriptorHeap, 128, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		RegisterDescriptorHeapAllocator(m_DSV_DescriptorHeap, 8, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		
		
		// Create command queue
		{
			D3D12_COMMAND_QUEUE_DESC desc = {};
			desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
			desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
			desc.NodeMask = 1;
			if (m_Device->CreateCommandQueue(&desc, IID_PPV_ARGS(&m_CommandQueue)) != S_OK)
				return false;
		}


		m_FrameContexts.resize(m_PipelineDesc.SwapChain.BufferCount);

		// Create a command allocator.
		for (int8 frameCntxIdx = 0; frameCntxIdx < m_PipelineDesc.SwapChain.BufferCount; ++frameCntxIdx)
		{
			// Create one command allocator for each back buffer
			hResult = m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_FrameContexts[frameCntxIdx].CommandAllocator));
			Assert(SUCCEEDED(hResult), "Failed to create the command allocator: \n(%u)", hResult);
		}


		// Create command list
		for (int8 frameCntxIdx = 0; frameCntxIdx < m_PipelineDesc.SwapChain.BufferCount; ++frameCntxIdx)
		{
			
		}
		
		// Initialize frame contexts
		for (int8 bufferIndex = 0; bufferIndex < m_PipelineDesc.SwapChain.BufferCount; ++bufferIndex)
		{
			
		}

		return 0;
	}

	int64 BorealisD3D12Renderer::SetupAssets()
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

}

#endif