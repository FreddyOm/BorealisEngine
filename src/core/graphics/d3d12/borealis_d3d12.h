#pragma once
#include "../../../config.h"
#include "../../types/types.h"

#if defined(BOREALIS_WIN)

#include <d3d12.h>
#include "../pipeline_config.h"

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
#define D3DCOMPILE_DEBUG 1
#endif

namespace Borealis::Graphics
{
	
	BOREALIS_API HRESULT InitializeD3D12(const PipelineDesc& pipelineConfig);
	BOREALIS_API HRESULT DeinitializeD3D12();
}

#endif