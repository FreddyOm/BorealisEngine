#include <gtest/gtest.h>
#include "../src/core/window/window.h"

TEST(WindowTest, WindowOpen)
{
    Borealis::Core::Window testWindow("Test Window");

    testWindow.OpenWindow();

    EXPECT_TRUE(testWindow.IsRunning());
    EXPECT_FALSE(testWindow.IsMinimized());

    EXPECT_GT(testWindow.GetWindowHeight(), 0);
    EXPECT_GT(testWindow.GetWindowWidth(), 0);
    
    testWindow.CloseWindow();
}