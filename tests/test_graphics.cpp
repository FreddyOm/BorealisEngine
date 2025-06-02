#include <gtest/gtest.h>
#include "../src/core/graphics/graphics.h"
#include "../src/core/window/window.h"

using namespace Borealis::Graphics;

TEST(D3D12GraphicsTest, InitPipeline)
{
	// Create window
	Borealis::Core::Window testWindow = Borealis::Core::Window("Graphics Test Window");

	testWindow.OpenWindow();

	// Create config
	PipelineDesc pipelineConfig{};
	pipelineConfig.SwapChain.WindowHandle = reinterpret_cast<HWND>(testWindow.GetWindowHandle());

	EXPECT_TRUE(SUCCEEDED(InitializeD3D12(pipelineConfig)));

	// Close window
	testWindow.CloseWindow();

}