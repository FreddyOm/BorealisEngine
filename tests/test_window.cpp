#include <gtest/gtest.h>
#include "../src/borealis_engine.h"
#include "../src/core/window/window.h"

#define VALUE(string) #string
#define TO_LITERAL(string) VALUE(string)

#ifdef BOREALIS_WIN

TEST(WindowTest, WindowOpenClose)
{
    //Borealis::Core::Window testWindow("Test Window");
    Borealis::Core::Window testWindow("BorealisTest - " + std::string(TO_LITERAL(BOREALIS_VERSION)));

    testWindow.OpenWindow();

    EXPECT_TRUE(testWindow.IsOpen());
    EXPECT_FALSE(testWindow.IsMinimized());
    EXPECT_FALSE(testWindow.IsMaximized());

    // TODO: Add maximization function and check if maximized is true!

    EXPECT_GT(testWindow.GetWindowHeight(), 0);
    EXPECT_GT(testWindow.GetWindowWidth(), 0);
    
#ifdef BOREALIS_WIN
    // The native handle can only be referenced by win targets for now. 
    // That is because the Vulkan implementation for Unix / OSX is not yet present in the code base.
    // As soon as the Vulkan impl. is ready, a native handle can be returned and the test case can be done for all platforms.
    EXPECT_NE(testWindow.GetNativeWindowHandle(), 0);
#endif
    testWindow.CloseWindow();

    EXPECT_FALSE(testWindow.IsOpen());

    EXPECT_FALSE(testWindow.IsMinimized());

    EXPECT_EQ(testWindow.GetWindowHeight(), 0);
    EXPECT_EQ(testWindow.GetWindowWidth(), 0);
}

#endif