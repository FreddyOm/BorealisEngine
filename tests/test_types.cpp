#include <gtest/gtest.h>
#include "../src/core/types/types.h"

using namespace Borealis::Types;

TEST(TypeTest, IntSize8) 
{
    EXPECT_EQ(sizeof(Int8), 1);
}

TEST(TypeTest, IntSize16) 
{
    EXPECT_EQ(sizeof(Int16), 2);
}

TEST(TypeTest, IntSize32) 
{
    EXPECT_EQ(sizeof(Int32), 4);
}

TEST(TypeTest, IntSize64) 
{
    EXPECT_EQ(sizeof(Int64), 8);
}


TEST(TypeTest, UintSize8)
{
    EXPECT_EQ(sizeof(Uint8), 1);
}

TEST(TypeTest, UintSize16)
{
    EXPECT_EQ(sizeof(Uint16), 2);
}

TEST(TypeTest, UintSize32)
{
    EXPECT_EQ(sizeof(Uint32), 4);
}

TEST(TypeTest, UintSize64)
{
    EXPECT_EQ(sizeof(Uint64), 8);
}