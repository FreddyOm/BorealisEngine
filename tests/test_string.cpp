#include <gtest/gtest.h>
#include "../src/core/types/string_id.h"

using namespace Borealis::Types;

#ifdef BOREALIS_DEBUG	// Is being optimized away in RelWithDebInfo

TEST(StringTest, DefaultInitString)
{
	StringId myString;
	StringId myString2;

	EXPECT_EQ(myString, myString2);
}

#endif

TEST(StringTest, StringEquality)
{
	StringId myString = String("MyFirstTestString");
	StringId myString2 = String("MySecondTestString");
	StringId myString3 = String("MyFirstTestString");
	StringId myString4 = String("MySecondTestString");

	EXPECT_NE(myString, myString4);
	EXPECT_NE(myString, myString2);
	EXPECT_EQ(myString, myString3);
	EXPECT_EQ(myString2, myString4);
}

TEST(StringTest, StringHashCollision)
{
	StringId myString = String("MyFirstTestString");
	StringId myString2 = String("MyFirstTestString ");
	StringId myString3 = String("MyFirstTestString.");
	StringId myString4 = String("MYFirstTestString");
	StringId myString5 = String(" MyFirstTestString");
	StringId myString6 = String("MYFIRSTTESTSTRING");

	EXPECT_NE(myString, myString2);
	EXPECT_NE(myString, myString3);
	EXPECT_NE(myString, myString4);
	EXPECT_NE(myString, myString5);
	EXPECT_NE(myString, myString6);
}