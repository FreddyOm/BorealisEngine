#include <gtest/gtest.h>
#include "../memory/stack_allocator.h"

using namespace Borealis::Memory;

TEST(AllocatorTest, Alloc)
{
	StackAllocator* allocator = nullptr;
	{
		StackAllocator testAllocator(1024);
		allocator = &testAllocator;

		long long* testLongLong = testAllocator.Alloc<long long>();
		
		*testLongLong = 42;

		EXPECT_TRUE(testLongLong != nullptr);
		EXPECT_GT(testAllocator.GetUsedMemorySize(), 0);

		EXPECT_EQ(testAllocator.GetUsedMemorySize(), 8);
	}

	EXPECT_EQ(allocator->GetUsedMemorySize(), 0);
}
