#include <gtest/gtest.h>
#include "../src/core/types/string_id.h"

using namespace Borealis::Types;

TEST(StringTest, DefaultInitString)
{
	StringId myString = String("");
	StringId myString2;
	StringId myString3;
	StringId myString4 = String("");

	EXPECT_EQ(myString, myString2);
	EXPECT_EQ(myString2, myString3);
	EXPECT_EQ(myString3, myString4);
	EXPECT_EQ(myString, myString4);
}

TEST(StringTest, StringEquality)
{
	StringId myString = String("MyFirstTestString");
	StringId myString2 = String("MySecondTestString");
}