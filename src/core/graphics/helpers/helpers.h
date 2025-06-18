#pragma once
#include "../../../config.h"
#include "../pipeline_config.h"
//#include "../../debug/runtime-debug/runtime-debug.h"

namespace Borealis::Graphics::Helpers
{
	struct BOREALIS_API IBorealisRenderer
	{
		IBorealisRenderer(const GraphicsBackend backend)
			: GraphicsBackend(backend)
		{ }
		
		virtual Borealis::Types::int64 InitializePipeline(const PipelineDesc& pipelineConfig) = 0;
		virtual Borealis::Types::int64 DeinitializePipeline() = 0;
		
		virtual const PipelineDesc& const GetPipelineDesc() const = 0;


		GraphicsBackend GraphicsBackend = GraphicsBackend::UNDEFINED;

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

		//Borealis::Runtime::Debug::IGUIDrawable* pRuntimeDebugger = nullptr;

#endif
	};
}