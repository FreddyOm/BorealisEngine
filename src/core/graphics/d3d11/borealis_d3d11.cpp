#include "borealis_d3d11.h"
#include "../../debug/logger.h"

#if defined(BOREALIS_WIN)

namespace Borealis::Graphics
{
    BorealisD3D11Renderer::~BorealisD3D11Renderer()
    {
#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
        Assert(!m_isInitialized,
            "Pipeline is not deinitialized! Make sure to call IBorealisRenderer::DeinitializePipeline() before destroying the renderer.");
#endif
    }

    Borealis::Types::int64 BorealisD3D11Renderer::InitializePipeline(const PipelineDesc& pipelineConfig)
    {
        m_PipelineConfiguration = pipelineConfig;

        return 0;
    }

    Borealis::Types::int64 BorealisD3D11Renderer::DeinitializePipeline()
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

    ID3D11Device* const BorealisD3D11Renderer::GetDevice() const
    {
        return m_Device.Get();
    }
    
    const PipelineDesc& const BorealisD3D11Renderer::GetPipelineDesc() const
    {
        return m_PipelineConfiguration;
    }
}

#endif