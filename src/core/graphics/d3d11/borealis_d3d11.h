#pragma once
#include "../../../config.h"
#include "../../types/types.h"

#if defined(BOREALIS_WIN)

#include "d3d11_common.h"
#include "../helpers/helpers.h"

namespace Borealis::Graphics
{
	struct BOREALIS_API BorealisD3D11Renderer : protected Helpers::IBorealisRenderer
	{
		BorealisD3D11Renderer(PipelineDesc& const pipelineDesc)
			: IBorealisRenderer(GraphicsBackend::D3D11, pipelineDesc)
		{ }
		~BorealisD3D11Renderer();

		BorealisD3D11Renderer(const BorealisD3D11Renderer& other) = delete;
		BorealisD3D11Renderer(BorealisD3D11Renderer&& other) noexcept = delete;
		BorealisD3D11Renderer& operator=(const BorealisD3D11Renderer& other) = delete;
		BorealisD3D11Renderer& operator=(BorealisD3D11Renderer&& other) noexcept = delete;


		Borealis::Types::int64 InitializePipeline() override;
		Borealis::Types::int64 DeinitializePipeline() override;

		ID3D11Device* const GetDevice() const;
		IDXGISwapChain4* const GetSwapChain() const;
		
	protected:

		Microsoft::WRL::ComPtr<ID3D11Device> m_Device{};
		
#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

	protected:

		Microsoft::WRL::ComPtr<ID3D11Debug> m_DebugController;
		Microsoft::WRL::ComPtr<IDXGIDebug1> m_DXGIDebug;

	private:

		bool m_isInitialized = false;
#endif
	};
}

#endif