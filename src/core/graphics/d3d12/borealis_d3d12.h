#pragma once
#include "../../../config.h"
#include "../../types/types.h"

#if defined(BOREALIS_WIN)
#include <vector>
#include "../helpers/d3d12_helpers.h"
#include "../helpers/helpers.h"
#include "../pipeline_config.h"

namespace Borealis::Graphics
{
	extern Borealis::Graphics::Helpers::D3D12DescriptorHeapAllocator g_RTVDescHeapAllocator;
	extern Borealis::Graphics::Helpers::D3D12DescriptorHeapAllocator g_SRVDescHeapAllocator;
	extern Borealis::Graphics::Helpers::D3D12DescriptorHeapAllocator g_DSVDescHeapAllocator;

	// TODO: Move this to helpers / general renderer file and make this activate debug layer depending on the backend used currently
	extern BOREALIS_API void InitD3D12LiveObjects();
	extern BOREALIS_API void ReportD3D12LiveObjects();

	struct BOREALIS_API BorealisD3D12Renderer : public Helpers::IBorealisRenderer
	{
		BorealisD3D12Renderer(PipelineDesc& pipelineDesc)
			: IBorealisRenderer(GraphicsBackend::D3D12, pipelineDesc)
		{ }
		~BorealisD3D12Renderer();

		BorealisD3D12Renderer(const BorealisD3D12Renderer& other) = delete;
		BorealisD3D12Renderer(BorealisD3D12Renderer&& other) noexcept = delete;
		BorealisD3D12Renderer& operator=(const BorealisD3D12Renderer& other) = delete;
		BorealisD3D12Renderer& operator=(BorealisD3D12Renderer&& other) noexcept = delete;
		
		Borealis::Types::int64 InitializePipeline() override;
		Borealis::Types::int64 DeinitializePipeline() override;

		ID3D12Device8* const GetDevice() const;
		ID3D12CommandQueue* const GetCommandQueue() const;
		ID3D12GraphicsCommandList7* const GetCommandList() const;
		IDXGISwapChain4* const GetSwapChain() const;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> GetDescriptorHeap(const D3D12_DESCRIPTOR_HEAP_TYPE descHeapType) const;
		ID3D12Resource* const GetRenderTarget(const Types::int32) const;
		ID3D12Resource* const GetCurrentRenderTarget() const;
		Helpers::FrameContext* const WaitForNextFrameContext();
		D3D12_CPU_DESCRIPTOR_HANDLE& GetRTVDescriptor(const Types::int32 rtvDescIdx);

		HRESULT PresentFrame();
		bool ToggleFullscreen();

		Microsoft::WRL::ComPtr<ID3D12Fence> m_CommandQueueFence;
		Types::uint64 m_LastSignaledFenceValue = 0;

		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList7> m_CommandList;

	private:

		Borealis::Types::int64 SetupPipeline();
		Borealis::Types::int64 SetupAssets();
		void WaitForPendingOperations();
		HRESULT RegisterDescriptorHeapAllocator(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& descHeap, const Types::int16 numDescriptors,
			const D3D12_DESCRIPTOR_HEAP_TYPE heapType, const Types::uint8 nodeMask = 0);
		HRESULT CreateRenderTargets();

	protected:

		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;

		std::vector<Helpers::FrameContext> m_FrameContexts = {};

		Microsoft::WRL::ComPtr<ID3D12Device8> m_Device;

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTV_DescriptorHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SRV_DescriptorHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSV_DescriptorHeap;
		
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_RenderTargets = 
			::std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>();
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_RTV_DescriptorHandles = 
			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>();

		// Synchronization objects
		HANDLE m_FenceEvent = nullptr;
		HANDLE m_SwapChainWaitable = nullptr;

		bool m_Fullscreen = false;
		
		// Evaluate if necessary
		bool m_VSync = true;
		bool m_SwapChainOccluded = false;
		bool m_SwapChainTearingSupport = false;
		Types::uint64 m_FrameIndex = 0;
		Types::int16 m_CurrentFrameContextIdx = 0;

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
		
	private:
		
		bool m_isInitialized = false;
#endif
	};
}

#endif