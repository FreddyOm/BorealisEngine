#include <gtest/gtest.h>
#include "../src/core/debug/logger.h"

using namespace Borealis::Debug;

// ------------------------- LOG -------------------------

TEST(DebugTest, TestLog)
{
	EXPECT_NO_FATAL_FAILURE(Log("Test Log"));

	EXPECT_NO_FATAL_FAILURE(Log("Test message"));
	EXPECT_NO_FATAL_FAILURE(Log(""));
	EXPECT_NO_FATAL_FAILURE(Log("LoGGING Some 8320384 more!!"));
	
	EXPECT_NO_FATAL_FAILURE(Log("Test %s", "Log"));
	EXPECT_NO_FATAL_FAILURE(Log("1 + 1 = %u", 2));
}

TEST(DebugTest, TestWarning)
{
	EXPECT_NO_FATAL_FAILURE(LogWarning("Test Warning"));
	EXPECT_NO_FATAL_FAILURE(LogWarning(""));
	EXPECT_NO_FATAL_FAILURE(LogWarning("LoGGING Some test more!!"));

	EXPECT_NO_FATAL_FAILURE(LogWarning("Test %s", "Warning"));
	EXPECT_NO_FATAL_FAILURE(LogWarning("1 + 1 = %u", 2));
}

TEST(DebugTest, TestError)
{
	EXPECT_NO_FATAL_FAILURE(LogError("Test Warning"));
	EXPECT_NO_FATAL_FAILURE(LogError(""));
	EXPECT_NO_FATAL_FAILURE(LogError("LoGGING Some test more!!"));

	EXPECT_NO_FATAL_FAILURE(LogError("Test %s", "Warning"));
	EXPECT_NO_FATAL_FAILURE(LogError("1 + 1 = %u", 2));
}

TEST(DebugTest, TestAssert)
{
#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
	
	EXPECT_NO_FATAL_FAILURE(Assert(0 == 0, "Test Warning"));
	EXPECT_DEATH(Assert(1 == 2, "1 is not equal to 2"), "");
	EXPECT_NO_FATAL_FAILURE(Assert(true || false, "LoGGING Some test more!!"));
	EXPECT_NO_FATAL_FAILURE(Assert(37 == 37, "Math doesn't work anymore!"));

#endif
}