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

    EXPECT_TRUE(testWindow.IsRunning());
    EXPECT_FALSE(testWindow.IsMinimized());

    EXPECT_GT(testWindow.GetWindowHeight(), 0);
    EXPECT_GT(testWindow.GetWindowWidth(), 0);
    
    EXPECT_NE(testWindow.GetWindowHandle(), 0);

    testWindow.CloseWindow();

    EXPECT_FALSE(testWindow.IsRunning());
    EXPECT_FALSE(testWindow.IsMinimized());

    EXPECT_EQ(testWindow.GetWindowHeight(), 0);
    EXPECT_EQ(testWindow.GetWindowWidth(), 0);
}

#endif