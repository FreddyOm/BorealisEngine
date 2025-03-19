#include <gtest/gtest.h>
#include "../src/core/types/types.h"

using namespace Borealis::Types;

TEST(TypeTest, IntSize8) 
{
    EXPECT_EQ(sizeof(int8), 1);
}

TEST(TypeTest, IntSize16) 
{
    EXPECT_EQ(sizeof(int16), 2);
}

TEST(TypeTest, IntSize32) 
{
    EXPECT_EQ(sizeof(int32), 4);
}

TEST(TypeTest, IntSize64) 
{
    EXPECT_EQ(sizeof(int64), 8);
}


TEST(TypeTest, UintSize8)
{
    EXPECT_EQ(sizeof(uint8), 1);
}

TEST(TypeTest, UintSize16)
{
    EXPECT_EQ(sizeof(uint16), 2);
}

TEST(TypeTest, UintSize32)
{
    EXPECT_EQ(sizeof(uint32), 4);
}

TEST(TypeTest, UintSize64)
{
    EXPECT_EQ(sizeof(uint64), 8);
}

TEST(TypeTest, IntPtr64)
{
    EXPECT_EQ(sizeof(int64Ptr), 8);
}

TEST(TypeTest, UintPtr64)
{
    EXPECT_EQ(sizeof(uint64Ptr), 8);

    uint64Ptr testPtr = 0;
    --testPtr;

    EXPECT_GT(testPtr, 0);
}