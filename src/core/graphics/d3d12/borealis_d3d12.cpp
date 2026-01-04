#include "borealis_d3d12.h"
#include "../../debug/logger.h"
#include "../../memory/memory.h"
#include "../d3d12/d3d12_common.h"
#include "../../debug/profiling.h"
//#include "../../debug/runtime-debug/EditorWindow.h"

using namespace Borealis::Types;

#if defined(BOREALIS_WIN)	// D3D12 only available for Windows OS

using namespace Microsoft::WRL;
using namespace Borealis::Graphics::Helpers;

namespace Borealis::Graphics
{
	Borealis::Graphics::Helpers::D3D12DescriptorHeapAllocator g_RTVDescHeapAllocator{};
	Borealis::Graphics::Helpers::D3D12DescriptorHeapAllocator g_SRVDescHeapAllocator{};
	Borealis::Graphics::Helpers::D3D12DescriptorHeapAllocator g_DSVDescHeapAllocator{};

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

	Microsoft::WRL::ComPtr<ID3D12Debug> g_DebugController;
	/*Microsoft::WRL::ComPtr<IDXGIDebug1> g_DXGIDebug;*/
	bool g_ReportLiveObjInitialized = false;

#endif

	BorealisD3D12Renderer::~BorealisD3D12Renderer()
	{
#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
		Assert(!m_isInitialized, 
			"Pipeline is not deinitialized! Make sure to call IBorealisRenderer::DeinitializePipeline() before destroying the renderer.");
#endif		
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
		// Wait for all pending operations to finish before destroying resources
		WaitForPendingOperations();

		// Set fullscreen state on swap chain
		if (m_SwapChain)
		{
			m_SwapChain->SetFullscreenState(false, nullptr);
		}
		
		if (m_SwapChainWaitable != nullptr) 
		{ 
			CloseHandle(m_SwapChainWaitable); 
		}

		// Release frame contexts
		for (auto& frameCntx : m_FrameContexts)
		{
			//frameCntx.CommandAllocator->Release();	// Not necessary scince ComPtr already calls Release!

			frameCntx.FenceValue = 0;
		}

		// Release command queue
		if (m_CommandQueue)
		{
			//m_CommandQueue.Get()->Release();
		}
		
		if (m_FenceEvent)
			CloseHandle(m_FenceEvent);

		g_SRVDescHeapAllocator.Destroy();
		g_DSVDescHeapAllocator.Destroy();
		g_RTVDescHeapAllocator.Destroy();

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
		m_isInitialized = false;

#endif
		return 0;
	}

	ID3D12Device8* const BorealisD3D12Renderer::GetDevice() const
	{
		return m_Device.Get();
	}

	ID3D12CommandQueue* const BorealisD3D12Renderer::GetCommandQueue() const
	{
		return m_CommandQueue.Get();
	}

	ID3D12GraphicsCommandList7* const BorealisD3D12Renderer::GetCommandList() const
	{
		return m_CommandList.Get();
	}

	IDXGISwapChain4* const BorealisD3D12Renderer::GetSwapChain() const
	{
		return m_SwapChain.Get();
	}

	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> BorealisD3D12Renderer::GetDescriptorHeap(const D3D12_DESCRIPTOR_HEAP_TYPE descHeapType) const
	{
		// TODO: consteval?
		switch (descHeapType)
		{
		case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
			return m_SRV_DescriptorHeap;
		case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
			return m_DSV_DescriptorHeap;
		case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
			return m_RTV_DescriptorHeap;
		}

		LogError("Couldn't resolve the correct descriptor heap for type %s. Returning RTV as default value.", descHeapType);
		return m_RTV_DescriptorHeap.Get();
	}

	ID3D12Resource* const BorealisD3D12Renderer::GetRenderTarget(const Types::int32 renderTargetIndex) const
	{
		Assert(renderTargetIndex < m_RenderTargets.size(),
			"Invalid index for accessing the render targets!");
		return m_RenderTargets.at(renderTargetIndex).Get();
	}

	ID3D12Resource* const BorealisD3D12Renderer::GetCurrentRenderTarget() const
	{
		return m_RenderTargets.at(m_SwapChain->GetCurrentBackBufferIndex()).Get();
	}

	/// <summary>
	/// Waits until the next frame context is available and returns it.
	/// </summary>
	/// <returns>A pointer to the next frame context.</returns>
	Helpers::FrameContext* const BorealisD3D12Renderer::WaitForNextFrameContext()
	{
		FrameContext* frame_context = &m_FrameContexts.at(m_FrameIndex % m_PipelineDesc.SwapChain.BufferCount);
		if (m_CommandQueueFence->GetCompletedValue() < frame_context->FenceValue)
		{
			m_CommandQueueFence->SetEventOnCompletion(frame_context->FenceValue, m_FenceEvent);
			HANDLE waitableObjects[] = { m_SwapChainWaitable, m_FenceEvent };
			::WaitForMultipleObjects(2, waitableObjects, TRUE, INFINITE);
		}
		else
		{
			::WaitForSingleObject(m_SwapChainWaitable, INFINITE);
		}

		return frame_context;
	}

	D3D12_CPU_DESCRIPTOR_HANDLE& BorealisD3D12Renderer::GetRTVDescriptor(const Types::int32 rtvDescIdx)
	{
		return m_RTV_DescriptorHandles.at(rtvDescIdx);
	}

	/// <summary>
	/// Initializes the D3D112 Info Queue for debug message handling.
	/// </summary>
	/// <param name="m_Device">The rendering pipeline device.</param>
	void InitD3D12InfoQueue(const ComPtr<ID3D12Device8> m_Device)
	{
#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

		HRESULT hResult{};

		if (g_ReportLiveObjInitialized && g_DebugController.Get() != nullptr)
		{
			Assert(m_Device.Get() != nullptr,
				"Device is NULL. Cannot query debug info queue from an invalid device");

			ComPtr<ID3D12InfoQueue1> infoQueue = nullptr;
			hResult = m_Device->QueryInterface(IID_PPV_ARGS(infoQueue.GetAddressOf()));

			Assert(SUCCEEDED(hResult),
				"Failed to query ID3D12InfoQueue: %s", StrFromHResult(hResult));

			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_ERROR, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_CORRUPTION, true);
			infoQueue->SetBreakOnSeverity(D3D12_MESSAGE_SEVERITY_WARNING, true);

			// Disable breaking on this warning because of a suspected bug in the D3D12 SDK layer.
			const int D3D12_MESSAGE_ID_FENCE_ZERO_WAIT_ = 1424; // not in all copies of d3d12sdklayers.h
			D3D12_MESSAGE_ID disabledMessages[] = { (D3D12_MESSAGE_ID)D3D12_MESSAGE_ID_FENCE_ZERO_WAIT_ };
			D3D12_INFO_QUEUE_FILTER filter = {};
			filter.DenyList.NumIDs = 1;
			filter.DenyList.pIDList = disabledMessages;
			infoQueue->AddStorageFilterEntries(&filter);

			//pInfoQueue->Release();
			g_DebugController->Release();
		}
		else
		{
			LogWarning("Debug Info Queue for D3D12 could not be initialized because the Debug Layer was not created correctly!");
		}
#endif
	}

	/// <summary>
	/// Presents a frame to the display.
	/// </summary>
	/// <returns>Result of the present operation. Returns 'S_OK' if successful, error codes if unsuccessful.</returns>
	HRESULT BorealisD3D12Renderer::PresentFrame()
	{
		// TODO: See https://learn.microsoft.com/en-us/windows/win32/direct3ddxgi/dxgi-status for future feature implementation!

		// Present
		static HRESULT hr;
		
		if (m_VSync)
		{
			hr = m_SwapChain->Present(1, 0);
		}
		else
		{
			hr = m_SwapChain->Present(0, m_SwapChainTearingSupport ? DXGI_PRESENT_ALLOW_TEARING : 0); // Present without vsync
		}
		
		m_SwapChainOccluded = (hr == DXGI_STATUS_OCCLUDED);
		++m_FrameIndex;

		return hr;
	}

	/// <summary>
	/// Sets the fullscreen state of the swap chain to the opposite of the current state.
	/// </summary>
	/// <returns>The new fullscreen state as boolean value.</returns>
	bool BorealisD3D12Renderer::ToggleFullscreen()
	{
		m_Fullscreen = !m_Fullscreen;
		
		{
			m_SwapChain->SetFullscreenState(m_Fullscreen, nullptr);

			// TODO: Resize the buffers if necessary
		}

		return m_Fullscreen;
	}

	/// <summary>
	/// Registers a descriptor heap allocator for the specified descriptor heap type.
	/// </summary>
	/// <param name="descHeap">The native descriptor heap to take ownership of.</param>
	/// <param name="numDescriptors">The amount of descriptor "slots" that can be stored in the heap.</param>
	/// <param name="heapType">The heap type, that defines to which allocator the heap is added!</param>
	/// <param name="NodeMask">The adapter, the resource is associated  with.</param>
	HRESULT BorealisD3D12Renderer::RegisterDescriptorHeapAllocator(ComPtr<ID3D12DescriptorHeap>& descHeap, const int16 numDescriptors,
		const D3D12_DESCRIPTOR_HEAP_TYPE heapType, const uint8 nodeMask)	
	{
		HRESULT hResult = S_OK;
		
		Assert(m_Device.Get() != nullptr, "Cannot register descriptor heap allocator when device is null!");
		Assert(numDescriptors > 0, "Number of descriptors must be greater than zero!");
		Assert(descHeap.Get() == nullptr, "Descriptor heap is already registered!");

		D3D12_DESCRIPTOR_HEAP_DESC descriptorHeapDesc = {};
		descriptorHeapDesc.Type = heapType;
		descriptorHeapDesc.NumDescriptors = numDescriptors;
		
		switch (heapType)
		{
			case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
			{
				descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
				break;
			}
			default:
			{
				descriptorHeapDesc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
				break;
			}
		}

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

	HRESULT BorealisD3D12Renderer::CreateRenderTargets()
	{
		HRESULT hResult{};
		
		Assert(m_RTV_DescriptorHandles.size() == m_PipelineDesc.SwapChain.BufferCount, "The number of RTV descriptor handles did not match the number of render targets to create!");

		m_RenderTargets.resize(m_PipelineDesc.SwapChain.BufferCount);

		for (Types::int16 i = 0; i < m_PipelineDesc.SwapChain.BufferCount; i++)
		{
			ComPtr<ID3D12Resource> backBuffer = nullptr;
			hResult = m_SwapChain->GetBuffer(i, IID_PPV_ARGS(backBuffer.GetAddressOf()));
			m_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, m_RTV_DescriptorHandles[i]);
			Assert(backBuffer != nullptr, "Unable to create the render target view!");
			m_RenderTargets[i] = backBuffer;
		}
		
		return hResult;
	}

	int64 BorealisD3D12Renderer::SetupPipeline()
	{
		Assert(m_PipelineDesc.SwapChain.WindowHandle != 0,
			"Invalid window handle! Make sure to create and initialize the window before initializing the pipeline!");

		HRESULT hResult{};
		ComPtr<IDXGIFactory7> m_DXGIFactory;

		// Create factory to define how objects are created
#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
		hResult = CreateDXGIFactory2(DXGI_CREATE_FACTORY_DEBUG, IID_PPV_ARGS(m_DXGIFactory.GetAddressOf()));
#else
		hResult = CreateDXGIFactory2(0, IID_PPV_ARGS(m_DXGIFactory.GetAddressOf()));
#endif

		Assert(SUCCEEDED(hResult), "Failed to create the dxgi factory: \n(%u)", hResult);

		{

			// Create the hardware adapter object
			ComPtr<IDXGIAdapter4> hardwareAdapter{};

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
		}

		// Init D3D12 debug layer
		InitD3D12InfoQueue(m_Device);

		// Create the command queue.
		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
		queueDesc.NodeMask = 1;

		hResult = m_Device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(&m_CommandQueue));
		Assert(SUCCEEDED(hResult), "Failed to create the command queue: \n(%u)", hResult);

		{
			// Create the swap chain.
			DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {};

			// Common desc
			swapChainDesc.AlphaMode = this->m_PipelineDesc.SwapChain.AlphaMode;
			swapChainDesc.BufferCount = this->m_PipelineDesc.SwapChain.BufferCount;
			swapChainDesc.BufferUsage = this->m_PipelineDesc.SwapChain.BufferUsage;
			swapChainDesc.Flags = DXGI_SWAP_CHAIN_FLAG_ALLOW_MODE_SWITCH | DXGI_SWAP_CHAIN_FLAG_FRAME_LATENCY_WAITABLE_OBJECT;
			swapChainDesc.Format = this->m_PipelineDesc.SwapChain.BufferFormat;
			swapChainDesc.Height = this->m_PipelineDesc.SwapChain.BufferHeight;
			swapChainDesc.SampleDesc.Count = this->m_PipelineDesc.SwapChain.SampleCount;
			swapChainDesc.SampleDesc.Quality = this->m_PipelineDesc.SwapChain.SampleQuality;
			swapChainDesc.Scaling = DXGI_SCALING_NONE;
			swapChainDesc.Stereo = false;
			swapChainDesc.SwapEffect = this->m_PipelineDesc.SwapChain.SwapEffect;
			swapChainDesc.Width = this->m_PipelineDesc.SwapChain.BufferWidth;

			// Swap chain full screen desc
			DXGI_SWAP_CHAIN_FULLSCREEN_DESC swapChainFSDesc = {};
			swapChainFSDesc.RefreshRate = { this->m_PipelineDesc.SwapChain.TargetRefreshRate, 1 };
			swapChainFSDesc.Scaling = this->m_PipelineDesc.SwapChain.ScaleMode;
			swapChainFSDesc.ScanlineOrdering = this->m_PipelineDesc.SwapChain.ScaleOrdering;
			swapChainFSDesc.Windowed = this->m_PipelineDesc.SwapChain.Windowed;

			// DXGI_FORMAT_R10G10B10A10_UNORM -> HDR 10 bit --> Not available?? Why?

			Assert((swapChainDesc.SwapEffect == DXGI_SWAP_EFFECT_FLIP_DISCARD || swapChainDesc.SwapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL) ?
				this->m_PipelineDesc.SwapChain.BufferFormat == DXGI_FORMAT_R16G16B16A16_FLOAT
				|| this->m_PipelineDesc.SwapChain.BufferFormat == DXGI_FORMAT_B8G8R8A8_UNORM
				|| this->m_PipelineDesc.SwapChain.BufferFormat == DXGI_FORMAT_R8G8B8A8_UNORM
				|| this->m_PipelineDesc.SwapChain.BufferFormat == DXGI_FORMAT_R10G10B10A2_UINT
				: true, "For the current swap effect, swap chain buffer-format must be set to %i, %i, %i or %i. However it is set to %i instead!",
				DXGI_FORMAT_R16G16B16A16_FLOAT,
				DXGI_FORMAT_B8G8R8A8_UNORM,
				DXGI_FORMAT_R8G8B8A8_UNORM,
				DXGI_FORMAT_R10G10B10A2_UINT,
				this->m_PipelineDesc.SwapChain.BufferFormat);

			Assert((swapChainDesc.SwapEffect == DXGI_SWAP_EFFECT_FLIP_DISCARD || swapChainDesc.SwapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL) ?
				this->m_PipelineDesc.SwapChain.SampleCount == 1
				|| this->m_PipelineDesc.SwapChain.SampleQuality == 0
				: true, "For the current swap effect, sample count and quality must be set to SampleCount = 1 and SampleQuality = 0!");

			Assert((swapChainDesc.SwapEffect == DXGI_SWAP_EFFECT_FLIP_DISCARD || swapChainDesc.SwapEffect == DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL) ?
				this->m_PipelineDesc.SwapChain.BufferCount >= 2
				&& this->m_PipelineDesc.SwapChain.BufferCount < 16
				: true, "For the current swap effect, the buffer count must be set between 2 and 16!");

			ComPtr<IDXGISwapChain1> swapChain;

#ifdef BOREALIS_CORE
			hResult = m_DXGIFactory->CreateSwapChain(m_CommandQueue.Get(), (HWND)m_PipelineConfiguration.SwapChain.WindowHandle, &swapChainDesc, &swapChain);
			Assert(SUCCEEDED(hResult), "Failed to create the swap chain: \n(%u)", hResult);
#else		// Windows 
			hResult = m_DXGIFactory->CreateSwapChainForHwnd(m_CommandQueue.Get(), (HWND)this->m_PipelineDesc.SwapChain.WindowHandle, &swapChainDesc, &swapChainFSDesc, NULL, &swapChain);
			Assert(SUCCEEDED(hResult), "Failed to create the swap chain: \n(%u)", hResult);
#endif

			hResult = swapChain.As(&m_SwapChain);
			Assert(SUCCEEDED(hResult), "Failed to cast swap chain: \n(%u)", hResult);

			hResult = m_SwapChain->SetMaximumFrameLatency(m_PipelineDesc.SwapChain.BufferCount);
			Assert(SUCCEEDED(hResult), "Failed to set maximum frame latency: \n(%u)", hResult);

			m_SwapChainWaitable = m_SwapChain->GetFrameLatencyWaitableObject();
		}
		// Init frame index
		m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

		hResult = m_DXGIFactory->MakeWindowAssociation((HWND) this->m_PipelineDesc.SwapChain.WindowHandle, 0);
		Assert(SUCCEEDED(hResult), "Failed to make the window association: \n(%u)", hResult);

		// TODO: Check if I need to create RTV and DSV descriptor heaps for the swap chain buffers
		// In ImGui example they are not created because RTV only uses the back buffers and therefore doesn't need to rely on more allocs

		
		// Create the render target view(RTV) descriptor heap.
		{
			hResult = RegisterDescriptorHeapAllocator(m_RTV_DescriptorHeap, 4, D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
			Assert(SUCCEEDED(hResult), "Failed to register RTV deschriptor heap allocator!");

			D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = m_RTV_DescriptorHeap->GetCPUDescriptorHandleForHeapStart();

			// Init descriptor handles for render targets
			m_RTV_DescriptorHandles.resize(this->m_PipelineDesc.SwapChain.BufferCount);

			for (Types::int64 bufferIdx = 0; bufferIdx < this->m_PipelineDesc.SwapChain.BufferCount; ++bufferIdx)
			{
				D3D12_CPU_DESCRIPTOR_HANDLE rtvHandle = {};
				g_RTVDescHeapAllocator.Alloc(&rtvHandle, nullptr);

				m_RTV_DescriptorHandles[bufferIdx] = rtvHandle;
			}
		}
		
		RegisterDescriptorHeapAllocator(m_SRV_DescriptorHeap, 128, D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV);
		Assert(SUCCEEDED(hResult), "Failed to register SRV deschriptor heap allocator!");


		// TODO: Use this when creating the Depth Stencil buffer for better performance!
		/*D3D12_CLEAR_VALUE clearValue;
		clearValue.Format = DXGI_FORMAT_D32_FLOAT;
		clearValue.DepthStencil.Depth = 1.0f;
		clearValue.DepthStencil.Stencil = 0;*/

		RegisterDescriptorHeapAllocator(m_DSV_DescriptorHeap, 8, D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
		Assert(SUCCEEDED(hResult), "Failed to register DSV deschriptor heap allocator!");
		
		m_FrameContexts.resize(this->m_PipelineDesc.SwapChain.BufferCount);
		
		// Create a command allocator.
		for (int8 frameCntxIdx = 0; frameCntxIdx < this->m_PipelineDesc.SwapChain.BufferCount; ++frameCntxIdx)
		{
			// Create one command allocator for each back buffer
			hResult = m_Device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_FrameContexts[frameCntxIdx].CommandAllocator));
			Assert(SUCCEEDED(hResult), "Failed to create the command allocator: \n(%u)", hResult);
		}
		
		// Create command list
		hResult = m_Device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_FrameContexts[m_FrameIndex].CommandAllocator.Get(), nullptr, IID_PPV_ARGS(&m_CommandList));
		Assert(SUCCEEDED(hResult), "Failed to create the command list: \n(%u)", hResult);

		// Immediately close the command list so it is not in recording state.
		hResult = m_CommandList->Close();
		Assert(SUCCEEDED(hResult), "Failed to close the command list: \n(%s)", StrFromHResult(hResult));
		
		// Initialize fence value
		hResult = m_Device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_CommandQueueFence));
		Assert(SUCCEEDED(hResult), "Failed to create a fence: \n(%u)", hResult);

		// Create fence event
		m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		Assert(m_FenceEvent != nullptr, "Failed to create fence events: \n(%u)", hResult);
		if (m_FenceEvent == nullptr)
		{
			hResult = HRESULT_FROM_WIN32(GetLastError());
			LogError(StrFromHResult(hResult));
		}


		// Create fence event
		m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		Assert(m_FenceEvent != nullptr, "Failed to create fence events: \n(%u)", hResult);

		CreateRenderTargets();
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

	/// <summary>
	/// Waits for all pending operations to finish.
	/// </summary>
	void BorealisD3D12Renderer::WaitForPendingOperations()
	{
		m_CommandQueue->Signal(m_CommandQueueFence.Get(), ++m_LastSignaledFenceValue);

		m_CommandQueueFence->SetEventOnCompletion(m_LastSignaledFenceValue, m_FenceEvent);
		::WaitForSingleObject(m_FenceEvent, INFINITE);
	}

	/// <summary>
	/// Initializes the report of live D3D12 objects.
	/// Only available in debug and relwithdebinfo builds. If called in release builds, the function does nothing.
	/// </summary>
	void InitD3D12LiveObjects()
	{
#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

		// TODO: Move this to helpers so it can be accessed afzer D3D12 shut down!
		// Enable the debug layer.
		HRESULT hResult = D3D12GetDebugInterface(IID_PPV_ARGS(&g_DebugController));
		
		if (SUCCEEDED(hResult))
		{
			g_DebugController->EnableDebugLayer();
			g_ReportLiveObjInitialized = true;
		}
		else
		{
			LogWarning("Could not enable debug layer in D3D12 backend: \n(us)", hResult);
		}
#endif
	}

	/// <summary>
	/// Prints all live objects referring to the D3D12 rendering pipeline. 
	/// Only works if 'InitD3D12LiveObjects' was called before initializing the rendering pipeline.
	/// Only available in debug and relwithdebinfo builds. If called in release builds, the function does nothing.
	/// </summary>
	void ReportD3D12LiveObjects()
	{
#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

		Assert(g_ReportLiveObjInitialized, 
			"Cannot report live objects if the debug layer was not successfully initialized. Make sure to call \"InitD3D12LiveObjects\" before initializing the rendering pipeline!");
		/*if (g_DXGIDebug)
		{
			g_DXGIDebug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_FLAGS(DXGI_DEBUG_RLO_SUMMARY | DXGI_DEBUG_RLO_IGNORE_INTERNAL));
		}*/

		ComPtr<IDXGIDebug1> debug = nullptr;
		if (SUCCEEDED(DXGIGetDebugInterface1(0, IID_PPV_ARGS(debug.GetAddressOf()))))
		{
			debug->ReportLiveObjects(DXGI_DEBUG_ALL, DXGI_DEBUG_RLO_SUMMARY);
		}

#endif
	}

}

#endif