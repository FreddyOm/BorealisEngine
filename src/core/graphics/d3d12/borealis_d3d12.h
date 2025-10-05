#pragma once
#include "../../../config.h"
#include "../../types/types.h"
#include "../../memory/memory.h"

#if defined(BOREALIS_WIN)
#include "d3d12_common.h"
#include <vector>
#include "../pipeline_config.h"
#include "../helpers/d3d12_helpers.h"

namespace Borealis::Graphics
{
	extern Borealis::Graphics::Helpers::D3D12DescriptorHeapAllocator g_RTVDescHeapAllocator;
	extern Borealis::Graphics::Helpers::D3D12DescriptorHeapAllocator g_SRVDescHeapAllocator;
	extern Borealis::Graphics::Helpers::D3D12DescriptorHeapAllocator g_DSVDescHeapAllocator;

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

		ID3D12Device* const GetDevice() const;
		ID3D12CommandQueue* const GetCommandQueue() const;
		ID3D12GraphicsCommandList* const GetCommandList() const;
		IDXGISwapChain4* const GetSwapChain() const;
		ID3D12DescriptorHeap* const GetDescriptorHeap() const;

		bool ToggleFullscreen();

	private:

		Borealis::Types::int64 SetupPipeline();
		Borealis::Types::int64 SetupAssets();
		void WaitForPendingOperations();
		HRESULT RegisterDescriptorHeapAllocator(Microsoft::WRL::ComPtr<ID3D12DescriptorHeap>& const descHeap, const Types::uint16 numDescriptors,
			const D3D12_DESCRIPTOR_HEAP_TYPE heapType, const Types::uint8 NodeMask = 0) const;
		

	protected:

		Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
		Microsoft::WRL::ComPtr<IDXGIFactory7> m_DXGIFactory;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandList> m_CommandList;

		std::vector<Helpers::FrameContext> m_FrameContexts = {};

		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTV_DescriptorHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SRV_DescriptorHeap;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSV_DescriptorHeap;
		
		std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_RenderTargets = 
			::std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>();
		std::vector<D3D12_CPU_DESCRIPTOR_HANDLE> m_RTV_DescriptorHandles = 
			std::vector<D3D12_CPU_DESCRIPTOR_HANDLE>();

		// Synchronization objects
		Microsoft::WRL::ComPtr<ID3D12Fence> m_CommandQueueFence;
		HANDLE m_FenceEvent = nullptr;
		Types::uint64 m_LastSignaledFenceValue = 0;

		bool m_Fullscreen = false;
		
		// Evaluate if necessary
		Types::uint64 m_FrameIndex = 0;
		Types::uint16 m_CurrentFrameContextIdx = 0;

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
		
	protected:

		Microsoft::WRL::ComPtr<ID3D12Debug> m_DebugController;
		Microsoft::WRL::ComPtr<IDXGIDebug1> m_DXGIDebug;

	private:
		
		bool m_isInitialized = false;
#endif
	};
}

#endif