#include <gtest/gtest.h>
#include "../src/core/debug/logger.h"

using namespace Borealis::Debug;

// ------------------------- LOG -------------------------

TEST(DebugTest, Log)
{
	EXPECT_GT(Log("Test Log"), 0);
	EXPECT_GT(Log("Test message"), 0);
	EXPECT_GT(Log("LoGGING Some 8320384 more!!"), 0);
	
	
	EXPECT_GT(Log("Test %s", "Log"), 0);
	EXPECT_GT(Log("1 + 1 = %u", 2), 0);
}