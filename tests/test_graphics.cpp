#include <core/graphics/graphics.h>
#include <core/window/window.h>

#include <gtest/gtest.h>

using namespace Borealis::Graphics;

#ifdef BOREALIS_WIN

TEST(D3D12GraphicsTest, InitAndDeinitPipeline)
{
    // Create window
    Borealis::Core::Window testWindow = Borealis::Core::Window("Graphics Test Window");

    testWindow.OpenWindow();

    // Create config
    PipelineDesc pipelineConfig {};
    pipelineConfig.SwapChain.WindowHandle = testWindow.GetNativeWindowHandle();

    // D3D12
#ifdef BOREALIS_WIN

    BorealisD3D12Renderer renderer = BorealisD3D12Renderer(pipelineConfig);

    EXPECT_EQ(0, renderer.InitializePipeline());

    EXPECT_EQ(0, renderer.DeinitializePipeline());

#endif

    // Close window
    testWindow.CloseWindow();
}

#endif