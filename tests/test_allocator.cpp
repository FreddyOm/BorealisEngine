#include <gtest/gtest.h>
#include "../memory/stack_allocator.h"
#include "../memory/pool_allocator.h"
#include "../types/types.h"

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

struct TestStruct	// 16 bytes
{
	Borealis::Types::int32 myInt = 0;
	bool myBoolArray[4] = {};
	float myFloatArray[2] = {};
};

TEST(PoolAllocatorTest, Alloc)
{
	PoolAllocator allocator = PoolAllocator(128, 512);

	EXPECT_EQ(allocator.GetAvailableMemorySize(), 128 * 512);
	EXPECT_EQ(allocator.GetTotalMemorySize(), 128 * 512);
	EXPECT_EQ(allocator.GetUsedMemorySize(), 0);

	TestStruct* p_myTestStruct = allocator.Alloc<TestStruct>();

	EXPECT_TRUE(p_myTestStruct != nullptr);

	p_myTestStruct->myInt = 3;
	
	p_myTestStruct->myBoolArray[0] = true;
	p_myTestStruct->myBoolArray[1] = false;
	p_myTestStruct->myBoolArray[2] = true;
	p_myTestStruct->myBoolArray[3] = false;

	p_myTestStruct->myFloatArray[0] = 3.14f;
	p_myTestStruct->myFloatArray[1] = 2.718f;

	EXPECT_EQ(allocator.GetAllocFreeRatio(), 1);
	EXPECT_GT(allocator.GetTotalMemorySize(), allocator.GetAvailableMemorySize());

	TestStruct* p_myOtherTestStruct = allocator.Alloc<TestStruct>();

	p_myOtherTestStruct->myInt = 3;

	p_myOtherTestStruct->myBoolArray[0] = true;
	p_myOtherTestStruct->myBoolArray[1] = false;
	p_myOtherTestStruct->myBoolArray[2] = true;
	p_myOtherTestStruct->myBoolArray[3] = false;

	p_myOtherTestStruct->myFloatArray[0] = 3.14f;
	p_myOtherTestStruct->myFloatArray[1] = 2.718f;

	EXPECT_EQ(allocator.GetAllocFreeRatio(), 2);
	EXPECT_GT(allocator.GetTotalMemorySize(), allocator.GetAvailableMemorySize());

	allocator.Free(p_myTestStruct);

	EXPECT_EQ(allocator.GetAllocFreeRatio(), 1);
	EXPECT_GT(allocator.GetTotalMemorySize(), allocator.GetAvailableMemorySize());
	EXPECT_EQ(allocator.GetUsedMemorySize(), 512);

	EXPECT_EQ(p_myOtherTestStruct->myInt, 3);
	EXPECT_EQ(p_myOtherTestStruct->myBoolArray[3], false);
	EXPECT_GT(p_myOtherTestStruct->myFloatArray[1], 0);
}

TEST(PoolAllocatorTest, Free)
{
	PoolAllocator allocator = PoolAllocator(2, 32);
	
	TestStruct* p_myTestStruct1 = allocator.Alloc<TestStruct>();
	EXPECT_TRUE(p_myTestStruct1 != nullptr);
	EXPECT_EQ(allocator.GetAllocFreeRatio(), 1);
	EXPECT_EQ(allocator.GetUsedMemorySize(), 32);

	allocator.Free(p_myTestStruct1);

	EXPECT_EQ(allocator.GetAllocFreeRatio(), 0);
	EXPECT_EQ(allocator.GetUsedMemorySize(), 0);

	// Terminates!
	EXPECT_DEATH(allocator.Free(p_myTestStruct1), "");

	EXPECT_EQ(allocator.GetAllocFreeRatio(), 0);
	EXPECT_EQ(allocator.GetUsedMemorySize(), 0);

	p_myTestStruct1 = allocator.Alloc<TestStruct>();

	EXPECT_EQ(allocator.GetAllocFreeRatio(), 1);
	EXPECT_EQ(allocator.GetUsedMemorySize(), 32);

	TestStruct* p_nonAllocMem = new TestStruct();
	EXPECT_DEATH(allocator.Free(p_nonAllocMem),"");

	EXPECT_EQ(allocator.GetAllocFreeRatio(), 1);
	EXPECT_EQ(allocator.GetUsedMemorySize(), 32);

	delete p_nonAllocMem;

	TestStruct* p_myTestStruct2 = allocator.Alloc<TestStruct>();
	p_myTestStruct2->myInt = 172782;
	allocator.Free(p_myTestStruct1);

	EXPECT_EQ(p_myTestStruct2->myInt, 172782);

	allocator.Free(p_myTestStruct2);
	
	EXPECT_EQ(allocator.GetAllocFreeRatio(), 0);
	EXPECT_EQ(allocator.GetUsedMemorySize(), 0);
	EXPECT_EQ(allocator.GetAvailableMemorySize(), allocator.GetTotalMemorySize());
}

TEST(PoolAllocatorTest, AllocFull)
{
	PoolAllocator allocator = PoolAllocator(2, 32);

	TestStruct* p_myTestStruct1 = allocator.Alloc<TestStruct>();
	TestStruct* p_myTestStruct2 = allocator.Alloc<TestStruct>();
	
	TestStruct* p_myTestStruct3 = allocator.Alloc<TestStruct>();
	EXPECT_EQ(p_myTestStruct3, nullptr);

	TestStruct* p_myTestStruct4 = allocator.Alloc<TestStruct>();
	EXPECT_EQ(p_myTestStruct4, nullptr);

	EXPECT_TRUE(p_myTestStruct1 != nullptr);
	EXPECT_TRUE(p_myTestStruct2 != nullptr);

	allocator.Clear();

	EXPECT_EQ(allocator.GetAvailableMemorySize(), allocator.GetTotalMemorySize());
	EXPECT_EQ(allocator.GetAllocFreeRatio(), 0);

	TestStruct* p_myNewTestStruct1 = allocator.Alloc<TestStruct>();
	TestStruct* p_myNewTestStruct2 = allocator.Alloc<TestStruct>();

	EXPECT_TRUE(p_myNewTestStruct1 != nullptr);
	EXPECT_TRUE(p_myNewTestStruct2 != nullptr);

	TestStruct* p_myNewTestStruct3 = allocator.Alloc<TestStruct>();

	EXPECT_TRUE(p_myNewTestStruct3 == nullptr);
}