#pragma once
#include "../../../config.h"
#include "../../types/types.h"

#if defined(BOREALIS_WIN)

#include "DirectX-Headers/include/directx/d3dx12.h"
#include <d3d12.h>
#include <wrl.h>
#include <vector>
#include <dxgi1_6.h>
#include <dxgiformat.h>
#include "../helpers/d3d12_helpers.h"
#include "../pipeline_config.h"


#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
#define D3DCOMPILE_DEBUG 1
#include <d3d12sdklayers.h>	// Debug Layer
#include <dxgidebug.h>
#endif



namespace Borealis::Graphics
{
	struct BOREALIS_API BorealisD3D12Renderer
	{
		BorealisD3D12Renderer(const PipelineDesc& pipelineConfig);
		~BorealisD3D12Renderer();

		BorealisD3D12Renderer(const BorealisD3D12Renderer& other) = delete;
		BorealisD3D12Renderer(BorealisD3D12Renderer&& other) noexcept = delete;
		BorealisD3D12Renderer& operator=(const BorealisD3D12Renderer& other) = delete;
		BorealisD3D12Renderer& operator=(BorealisD3D12Renderer&& other) noexcept = delete;
	
	private:
		HRESULT InitializeD3D12Pipeline(const PipelineDesc& pipelineConfig);
		HRESULT InitializeAssets();

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
		Microsoft::WRL::ComPtr<ID3D12Debug> m_DebugController;
		Microsoft::WRL::ComPtr<IDXGIDebug1> m_DXGIDebug;
#endif
	};
}

#endif