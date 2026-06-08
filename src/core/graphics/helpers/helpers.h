#pragma once
#include "../../../config.h"
#include "../../helpers/macros.h"
#include "../pipeline_config.h"
#include "../../types/types.h"

#ifdef BOREALIS_WIN
#include <d3d12.h>
#endif

namespace Borealis::Graphics::Helpers
{
	struct BOREALIS_API IBorealisRenderer
	{
		IBorealisRenderer(Borealis::Graphics::GraphicsBackend backend, const Borealis::Graphics::PipelineDesc& pipelineDesc)
			: m_GraphicsBackend(backend), m_PipelineDesc(pipelineDesc)
		{ }

		virtual ~IBorealisRenderer() = default;
		
        BOREALIS_DELETE_COPY_CONSTRUCT(IBorealisRenderer)
        BOREALIS_DELETE_MOVE_CONSTRUCT(IBorealisRenderer)
        BOREALIS_DELETE_COPY_ASSIGN(IBorealisRenderer)
        BOREALIS_DELETE_MOVE_ASSIGN(IBorealisRenderer)

		virtual Borealis::Types::int64 InitializePipeline() = 0;
		virtual Borealis::Types::int64 DeinitializePipeline() = 0;
		virtual void WaitForPendingOperations() = 0;
		
		const Borealis::Graphics::GraphicsBackend m_GraphicsBackend;
		const Borealis::Graphics::PipelineDesc m_PipelineDesc;
	};
}
