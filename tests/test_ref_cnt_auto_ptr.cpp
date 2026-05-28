#include <gtest/gtest.h>
#include <core/memory/ref_cnt_auto_ptr.h>
#include <core/types/types.h>
#include <core/memory/memory.h>

using namespace Borealis::Memory;
using namespace Borealis::Types;

struct TestStruct	// 16 bytes
{
	Borealis::Types::int32 myInt = 0;
	bool myBoolArray[4] = {};
	float myFloatArray[2] = {};
};

struct DerivedStruct : public TestStruct
{
	bool derivedBool = false;
	int derivedInt = 0;
};

TEST(RefCntAutoPtrTest, GeneralUse)
{
	MemAllocJanitor janitor(MemAllocatorContext::DEBUG);

	RefCntAutoPtr<TestStruct> myReference = RefCntAutoPtr<TestStruct>::Allocate();
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

TEST(RefCntAutoPtrTest, DefaultInit)
{
	MemAllocJanitor janitor(MemAllocatorContext::DEBUG);
	RefCntAutoPtr<TestStruct> myReference;
	
	EXPECT_EQ(myReference.RawPtr(), nullptr);
	EXPECT_EQ(myReference.UseCount(), 0);

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
	
	EXPECT_DEATH(myReference->myInt, ""); 
	EXPECT_DEATH(myReference->myBoolArray[0] = false, "");

#endif
}

TEST(RefCntAutoPtrTest, InvalidatedPtr)
{
	MemAllocJanitor janitor(MemAllocatorContext::DEBUG);

	RefCntAutoPtr<TestStruct> myReference = RefCntAutoPtr<TestStruct>::Allocate();
	myReference->myInt = 3;
	EXPECT_EQ(myReference->myInt, 3);

	// Manually invalidate pointer
	myReference.~RefCntAutoPtr();

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

	// Invalidated pointer should not allow accessing members
	EXPECT_EQ(myReference.RawPtr(), nullptr);

#endif
}

TEST(RefCntAutoPtrTest, InitPtrRef)
{
	MemAllocJanitor janitor(MemAllocatorContext::DEBUG);

	// "Normal" allocator init
	RefCntAutoPtr<TestStruct> myReference = RefCntAutoPtr<TestStruct>::Allocate();
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
	RefCntAutoPtr<TestStruct> myRef = RefCntAutoPtr<TestStruct>::Allocate();
	RefCntAutoPtr<TestStruct> myRefCpy = myRef;

	RefCntAutoPtr<TestStruct> myRef2 = RefCntAutoPtr<TestStruct>::Allocate();
	RefCntAutoPtr<TestStruct> myRef2Cpy(myRef2);


	EXPECT_EQ(myRef, myRefCpy);
	EXPECT_EQ(myRef2, myRef2Cpy);

	EXPECT_NE(myRef, myRef2Cpy);
	EXPECT_NE(myRef2, myRefCpy);
}

TEST(RefCntAutoPtrTest, RefPtrMemberAccess)
{
	MemAllocJanitor janitor(MemAllocatorContext::DEBUG);

	// "Normal" allocator init
	RefCntAutoPtr<TestStruct> testStruct = RefCntAutoPtr<TestStruct>::Allocate();

	testStruct->myBoolArray[0] = true;
	testStruct->myBoolArray[1] = false;
	testStruct->myBoolArray[2] = true;
	testStruct->myBoolArray[3] = false;

	testStruct->myFloatArray[0] = 2.718f;
	testStruct->myFloatArray[1] = 3.1415f;

	testStruct->myInt = -403928;


	EXPECT_TRUE(testStruct->myBoolArray[0]);
	EXPECT_FALSE(testStruct->myBoolArray[1]);
	EXPECT_TRUE(testStruct->myBoolArray[2]);
	EXPECT_FALSE(testStruct->myBoolArray[3]);

	EXPECT_FLOAT_EQ(testStruct->myFloatArray[0], 2.718f);
	EXPECT_FLOAT_EQ(testStruct->myFloatArray[1], 3.1415f);

	EXPECT_EQ(testStruct->myInt, -403928);
}

TEST(RefCntAutoPtrTest, IsValid)
{
	MemAllocJanitor janitor(MemAllocatorContext::DEFAULT);

	RefCntAutoPtr<TestStruct> testStruct = RefCntAutoPtr<TestStruct>::Allocate();

	testStruct->myBoolArray[0] = true;

	EXPECT_TRUE(testStruct->myBoolArray[0]);
	EXPECT_TRUE(testStruct.IsValid());
	EXPECT_TRUE(testStruct.RawPtr() != nullptr);

	testStruct = RefCntAutoPtr<TestStruct>();

	EXPECT_FALSE(testStruct.IsValid());
}

TEST(RefCntAutoPtrTest, DynamicCast)
{
	MemAllocJanitor janitor(MemAllocatorContext::DEFAULT);
	
	RefCntAutoPtr<DerivedStruct> derivedStruct = RefCntAutoPtr<DerivedStruct>::Allocate();

	derivedStruct->derivedBool = true;
	derivedStruct->myInt = 42;

	RefCntAutoPtr<TestStruct> testStruct = RefCntAutoPtr<DerivedStruct>::DynamicCastTo<TestStruct>(derivedStruct);

	EXPECT_TRUE(derivedStruct.IsValid());
	EXPECT_TRUE(testStruct.IsValid());

	EXPECT_TRUE(testStruct->myInt == 42);
	EXPECT_TRUE(testStruct.RawPtr() == derivedStruct.RawPtr());


	RefCntAutoPtr<DerivedStruct> derivedStruct2 = RefCntAutoPtr<TestStruct>::StaticCastTo<DerivedStruct>(testStruct);

	EXPECT_TRUE(derivedStruct2.IsValid());

	EXPECT_TRUE(derivedStruct2->myInt == 42);
	EXPECT_TRUE(derivedStruct2.RawPtr() == derivedStruct.RawPtr());
	EXPECT_TRUE(derivedStruct2.RawPtr() == testStruct.RawPtr());
	EXPECT_TRUE(derivedStruct2->derivedBool);
}