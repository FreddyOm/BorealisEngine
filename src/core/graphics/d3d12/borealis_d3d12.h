#pragma once
#include "../../../config.h"
#include "../../types/types.h"

#if defined(BOREALIS_WIN)

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif

#include <Windows.h>
#include <dxgi1_6.h>
#include <d3d12.h>
//#include "DirectX-Headers/include/directx/d3dx12.h"
#include "../helpers/helpers.h"
#include <dxgiformat.h>
//#include <d3dcommon.h>
#include <wrl.h>
#include <vector>
#include "../pipeline_config.h"


#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
#define D3DCOMPILE_DEBUG 1
#include <d3d12sdklayers.h>	// Debug Layer
#include <dxgidebug.h>
#endif

namespace Borealis::Graphics
{
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

	private:

		Borealis::Types::int64 SetupPipeline();
		Borealis::Types::int64 SetupAssets();

	protected:

		Microsoft::WRL::ComPtr<ID3D12Device> m_Device;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_CommandQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_CommandAllocator;
		Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVHeap;
		::std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>> m_RenderTargets = 
			::std::vector<Microsoft::WRL::ComPtr<ID3D12Resource>>();
		
		Microsoft::WRL::ComPtr<IDXGIFactory7> m_DXGIFactory;
		Microsoft::WRL::ComPtr<IDXGISwapChain4> m_SwapChain;

		Types::uint64 m_FrameIndex = 0;
		Types::uint64 m_RTVDescriptorSize = 0;

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