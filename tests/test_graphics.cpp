#include <gtest/gtest.h>
#include "../src/core/graphics/graphics.h"

using namespace Borealis::Graphics;

TEST(D3D12GraphicsTest, DebugLayer)
{
	PipelineDesc pipelineConfig =
	{
		D3D_FEATURE_LEVEL_11_0
	};

	EXPECT_TRUE(SUCCEEDED(InitializeD3D12(pipelineConfig)));

}