#pragma once
#include "../../../config.h"
#include "../pipeline_config.h"

namespace Borealis::Graphics::Helpers
{
	struct BOREALIS_API IBorealisRenderer
	{
		IBorealisRenderer(Borealis::Graphics::GraphicsBackend backend, const Borealis::Graphics::PipelineDesc& pipelineDesc)
			: m_GraphicsBackend(backend), m_PipelineDesc(pipelineDesc)
		{ }
		
		virtual Borealis::Types::int64 InitializePipeline() = 0;
		virtual Borealis::Types::int64 DeinitializePipeline() = 0;
		
		const Borealis::Graphics::GraphicsBackend m_GraphicsBackend;
		const Borealis::Graphics::PipelineDesc m_PipelineDesc;
	};
}