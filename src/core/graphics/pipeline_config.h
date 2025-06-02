#pragma once
#include "../../config.h"

namespace Borealis::Graphics
{

#ifdef BOREALIS_WIN

#include <d3d12.h>
#include <dxgi.h>
#include <windef.h>


	struct BOREALIS_API SwapChainConfig
	{
		Types::uint8 BufferCount = 2;
		Types::uint8 SampleCount = 1;
		Types::uint16 BufferWidth = 100;
		Types::uint16 BufferHeight = 100;
		DXGI_FORMAT BufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
		DXGI_USAGE BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		DXGI_SWAP_EFFECT SwapEffect = DXGI_SWAP_EFFECT_FLIP_DISCARD;
		HWND WindowHandle = 0;
		bool Windowed = true;
	};

	struct BOREALIS_API PipelineDesc
	{
		D3D_FEATURE_LEVEL MinimumFeatureLevel = D3D_FEATURE_LEVEL_11_0;
		D3D_FEATURE_LEVEL TargetFeatureLevel = D3D_FEATURE_LEVEL_12_2;
		SwapChainConfig SwapChain;
	};


#else

	// @TODO: Implement Vulkan Pipeline Desc
	struct PipelineDesc
	{

	};

#endif

}
