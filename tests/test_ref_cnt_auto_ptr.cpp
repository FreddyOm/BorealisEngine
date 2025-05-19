#include <gtest/gtest.h>
#include "../memory/ref_cnt_auto_ptr.h"
#include "../types/types.h"
#include "../src/core/memory/allocator.h"

using namespace Borealis::Memory;
using namespace Borealis::Types;

struct TestStruct	// 16 bytes
{
	Borealis::Types::int32 myInt = 0;
	bool myBoolArray[4] = {};
	float myFloatArray[2] = {};
};

TEST(RefCntAutoPtrTest, GeneralUse)
{
	MemAllocJanitor janitor(MemAllocatorContext::DEBUG);

	RefCntAutoPtr<TestStruct> myReference = Allocate<TestStruct>();
	myReference->myInt = 3;
	EXPECT_EQ(myReference->myInt, 3);

	{
		RefCntAutoPtr<TestStruct> myReferenceCpy = myReference;

		EXPECT_EQ(myReferenceCpy->myInt, 3);
		EXPECT_EQ(myReferenceCpy->myBoolArray[0], false);
		EXPECT_EQ(myReference.UseCount(), 2);

		myReferenceCpy->myFloatArray[1] = 3.1415f;

		// ref cpy falls out of scope and should be destroyed
	}

	EXPECT_EQ(myReference->myInt, 3);
	EXPECT_FLOAT_EQ(myReference->myFloatArray[1], 3.1415f);
	EXPECT_EQ(myReference.UseCount(), 1);
}

TEST(RefCntAutoPtrTest, InvalidatedPtr)
{
	MemAllocJanitor janitor(MemAllocatorContext::DEBUG);

	RefCntAutoPtr<TestStruct> myReference = Allocate<TestStruct>();
	myReference->myInt = 3;
	EXPECT_EQ(myReference->myInt, 3);

	// Manually invalidate pointer
	myReference.~RefCntAutoPtr();

	// Invalidated pointer should not allow accessing members
	EXPECT_DEATH(myReference->myInt, "");
	EXPECT_DEATH(myReference->myBoolArray[0], "");
}

TEST(RefCntAutoPtrTest, InitPtrRef)
{
	MemAllocJanitor janitor(MemAllocatorContext::DEBUG);

	// "Normal" allocator init
	RefCntAutoPtr<TestStruct> myReference = Allocate<TestStruct>();
	EXPECT_EQ(myReference.UseCount(), 1);


	// Assign constructor init
	RefCntAutoPtr<TestStruct> assignConstructorInitRef = myReference;
	EXPECT_EQ(myReference.UseCount(), 2);


	// Constructor init
	RefCntAutoPtr<TestStruct> ConstructorInitRef(assignConstructorInitRef);
	EXPECT_EQ(myReference.UseCount(), 3);


	// Move constructor init
	RefCntAutoPtr<TestStruct> moveAssignConstructorInitRef = std::move(ConstructorInitRef);
	EXPECT_EQ(myReference.UseCount(), 3);


	// Move constructor init
	RefCntAutoPtr<TestStruct> moveConstructorInitRef(std::move(moveAssignConstructorInitRef));
	EXPECT_EQ(myReference.UseCount(), 3);
}

TEST(RefCntAutoPtrTest, RefPtrEquality)
{
	MemAllocJanitor janitor(MemAllocatorContext::DEBUG);

	// "Normal" allocator init
	RefCntAutoPtr<TestStruct> myRef = Allocate<TestStruct>();
	RefCntAutoPtr<TestStruct> myRefCpy = myRef;

	RefCntAutoPtr<TestStruct> myRef2 = Allocate<TestStruct>();
	RefCntAutoPtr<TestStruct> myRef2Cpy(myRef2);


	EXPECT_EQ(myRef, myRefCpy);
	EXPECT_EQ(myRef2, myRef2Cpy);

	EXPECT_NE(myRef, myRef2Cpy);
	EXPECT_NE(myRef2, myRefCpy);
}

TEST(RefCntAutoPtrTest, RefPtrRawPtr)
{
	MemAllocJanitor janitor(MemAllocatorContext::DEBUG);

	// "Normal" allocator init
	TestStruct* p_TestStruct = Allocate<TestStruct>();
	RefCntAutoPtr<TestStruct> myRef = p_TestStruct;

	EXPECT_EQ(p_TestStruct->myInt, myRef->myInt);
	EXPECT_EQ(p_TestStruct, myRef.RawPtr());
	EXPECT_EQ(p_TestStruct, &myRef);
}

TEST(RefCntAutoPtrTest, RefPtrMemberAccess)
{
	MemAllocJanitor janitor(MemAllocatorContext::DEBUG);

	// "Normal" allocator init
	TestStruct* p_TestStruct = Allocate<TestStruct>();
	RefCntAutoPtr<TestStruct> myRef = p_TestStruct;

	myRef->myBoolArray[0] = true;
	myRef->myBoolArray[1] = false;
	myRef->myBoolArray[2] = true;
	myRef->myBoolArray[3] = false;

	myRef->myFloatArray[0] = 2.718f;
	myRef->myFloatArray[1] = 3.1415f;

	myRef->myInt = -403928;


	EXPECT_TRUE(p_TestStruct->myBoolArray[0]);
	EXPECT_FALSE(p_TestStruct->myBoolArray[1]);
	EXPECT_TRUE(p_TestStruct->myBoolArray[2]);
	EXPECT_FALSE(p_TestStruct->myBoolArray[3]);

	EXPECT_FLOAT_EQ(p_TestStruct->myFloatArray[0], 2.718f);
	EXPECT_FLOAT_EQ(p_TestStruct->myFloatArray[1], 3.1415f);

	EXPECT_EQ(p_TestStruct->myInt, -403928);
}