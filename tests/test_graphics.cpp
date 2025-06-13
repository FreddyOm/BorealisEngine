#include <gtest/gtest.h>
#include "../src/core/graphics/graphics.h"
#include "../src/core/window/window.h"

using namespace Borealis::Graphics;

#ifdef BOREALIS_WIN

TEST(D3D12GraphicsTest, InitPipeline)
{
	// Create window
	Borealis::Core::Window testWindow = Borealis::Core::Window("Graphics Test Window");

	testWindow.OpenWindow();

	// Create config
	PipelineDesc pipelineConfig{};
	pipelineConfig.SwapChain.WindowHandle = reinterpret_cast<HWND>(testWindow.GetWindowHandle());

	// D3D12
#ifdef BOREALIS_WIN

	EXPECT_TRUE(SUCCEEDED(InitializeD3D12(pipelineConfig)));

	DeinitializeD3D12();
#endif

	// Close window
	testWindow.CloseWindow();
}

#endif