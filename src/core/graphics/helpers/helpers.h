#pragma once
#include "../../../config.h"
#include "../../types/types.h"
#include "../pipeline_config.h"

namespace Borealis::Graphics::Helpers
{
	struct BOREALIS_API IBorealisRenderer
	{
		virtual Borealis::Types::int64 InitializePipeline(const PipelineDesc& pipelineConfig) = 0;
		virtual Borealis::Types::int64 DeinitializePipeline() = 0;
	};
}