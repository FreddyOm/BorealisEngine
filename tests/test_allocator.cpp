#include <gtest/gtest.h>
#include "../memory/stack_allocator.h"

using namespace Borealis::Memory;
using namespace Borealis::Types;

TEST(StackAllocatorTest, Alloc)
{
	StackAllocator* allocator = nullptr;
	{
		StackAllocator testAllocator(1024);
		allocator = &testAllocator;

		int64* testInt = testAllocator.Alloc<int64>();
		
		*testInt = 42;

		EXPECT_TRUE(testInt != nullptr);
		EXPECT_GT(testAllocator.GetUsedMemorySize(), 0);

		EXPECT_EQ(testAllocator.GetUsedMemorySize(), 8);
	}

	EXPECT_EQ(allocator->GetUsedMemorySize(), 0);
}


TEST(StackAllocatorTest, Free)
{
	StackAllocator testAllocator(1024);

	uint32* testObject32 = testAllocator.Alloc<uint32>();
	uint64* testObject64 = testAllocator.Alloc<uint64>();

	testAllocator.Free(testObject32);
	testAllocator.Free(testObject64);

	EXPECT_EQ(testAllocator.GetTotalMemorySize(), testAllocator.GetAvailableMemorySize());
	EXPECT_EQ(testAllocator.GetTotalMemorySize(), 1024);
	EXPECT_EQ(testAllocator.GetAvailableMemorySize(), 1024);
	EXPECT_EQ(testAllocator.GetUsedMemorySize(), 0);
}

TEST(StackAllocatorTest, AllocAligned)
{
	StackAllocator testAllocator(1024);

	uint32* testObject32 = testAllocator.AllocAligned<uint32>();
	uint64* testObject64 = testAllocator.AllocAligned<uint64>();

	// Check Alignment
	EXPECT_EQ((uint64) testObject32 % 4, 0);
	EXPECT_EQ((uint64) testObject64 % 8, 0);

	testAllocator.FreeAligned(testObject64);
	testAllocator.FreeAligned(testObject32);

	EXPECT_EQ(testAllocator.GetTotalMemorySize(), testAllocator.GetAvailableMemorySize());
	EXPECT_EQ(testAllocator.GetTotalMemorySize(), 1024);
}

TEST(StackAllocatorTest, AllocFreeRatio)
{
	StackAllocator testAllocator(1024);

	uint32* testObject32 = testAllocator.AllocAligned<uint32>();
	uint64* testObject64 = testAllocator.AllocAligned<uint64>();

	EXPECT_EQ(testAllocator.GetAllocFreeRatio(), 2);

	testAllocator.FreeAligned(testObject64);
	testAllocator.FreeAligned(testObject32);

	EXPECT_EQ(testAllocator.GetAllocFreeRatio(), 0);
	
	uint64* testObject64_v2 = testAllocator.AllocAligned<uint64>();

	EXPECT_EQ(testAllocator.GetAllocFreeRatio(), 1);

	bool* testBool_v1 = testAllocator.AllocAligned<bool>();
	bool* testBool_v2 = testAllocator.AllocAligned<bool>();
	bool* testBool_v3 = testAllocator.AllocAligned<bool>();

	testAllocator.FreeAligned(testBool_v3);

	EXPECT_EQ(testAllocator.GetAllocFreeRatio(), 3);

	bool* testBool_v4 = testAllocator.AllocAligned<bool>();
	bool* testBool_v5 = testAllocator.AllocAligned<bool>();
	bool* testBool_v6 = testAllocator.AllocAligned<bool>();

	testAllocator.FreeAligned(testBool_v6);
	testAllocator.FreeAligned(testBool_v5);
	testAllocator.FreeAligned(testBool_v4);
	testAllocator.FreeAligned(testBool_v2);
	testAllocator.FreeAligned(testBool_v1);

	testAllocator.FreeAligned(testObject64_v2);

	EXPECT_EQ(testAllocator.GetAllocFreeRatio(), 0);
}