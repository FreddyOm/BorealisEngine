#pragma once
#include "../../../config.h"
#include "../pipeline_config.h"
//#include "../../debug/runtime-debug/EditorWindow.h"

namespace Borealis::Graphics::Helpers
{
	struct BOREALIS_API IBorealisRenderer
	{
		IBorealisRenderer(const GraphicsBackend backend, PipelineDesc& const pipelineDesc)
			: GraphicsBackend(backend), PipelineDescription(pipelineDesc)
		{ }
		
		virtual Borealis::Types::int64 InitializePipeline() = 0;
		virtual Borealis::Types::int64 DeinitializePipeline() = 0;
		
		const GraphicsBackend GraphicsBackend = GraphicsBackend::UNDEFINED;
		const PipelineDesc PipelineDescription{};
	};
}