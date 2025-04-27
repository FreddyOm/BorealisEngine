#include <gtest/gtest.h>
#include "../src/core/debug/logger.h"

using namespace Borealis::Debug;

// ------------------------- LOG -------------------------

TEST(DebugTest, Log)
{
	EXPECT_GT(Log("Test Log"), 0);
	EXPECT_GT(Log("Test message"), 0);
	EXPECT_GT(Log(""), 0);
	EXPECT_GT(Log("LoGGING Some 8320384 more!!"), 0);
	
	
	EXPECT_GT(Log("Test %s", "Log"), 0);
	EXPECT_GT(Log("1 + 1 = %u", 2), 0);
}


TEST(DebugTest, Warning)
{
	EXPECT_GT(LogWarning("Test Warning"), 0);
	EXPECT_GT(LogWarning(""), 0);
	EXPECT_GT(LogWarning("LoGGING Some test more!!"), 0);


	EXPECT_GT(LogWarning("Test %s", "Warning"), 0);
	EXPECT_GT(LogWarning("1 + 1 = %u", 2), 0);
}

TEST(DebugTest, Error)
{
	EXPECT_GT(LogError("Test Warning"), 0);
	EXPECT_GT(LogError(""), 0);
	EXPECT_GT(LogError("LoGGING Some test more!!"), 0);


	EXPECT_GT(LogError("Test %s", "Warning"), 0);
	EXPECT_GT(LogError("1 + 1 = %u", 2), 0);
}

TEST(DebugTest, Assert)
{
	EXPECT_NO_FATAL_FAILURE(Assert(0 == 0, "Test Warning"));
	EXPECT_DEATH(Assert(1 == 2, "1 is not equal to 2"), "");
	EXPECT_NO_FATAL_FAILURE(Assert(true || false, "LoGGING Some test more!!"));
	EXPECT_NO_FATAL_FAILURE(Assert(37 == 37, "Math doesn't work anymore!"));
}