#include <gtest/gtest.h>
#include <core/memory/memory.h>
#include <core/memory/stack_allocator.h>
#include <core/memory/pool_allocator.h>
#include <core/memory/heap_allocator.h>
#include <core/memory/ref_cnt_auto_ptr.h>
#include <core/types/types.h>

using namespace Borealis::Memory;
using namespace Borealis::Types;

struct TestStruct	// 16 bytes
{
	Borealis::Types::int32 myInt = 0;
	bool myBoolArray[4] = {};
	float myFloatArray[2] = {};
};

struct BigStruct	// 32 bytes
{
	TestStruct testStructs[2];
};

TEST(AllocatorTest, Janitor)
{
	// Test if Janitors push and pop allocators correctly

	EXPECT_TRUE(g_memoryAllocatorContext.empty());

	{
		MemAllocJanitor janitor(MemAllocatorContext::DEBUG);

		EXPECT_EQ(g_memoryAllocatorContext.top(), MemAllocatorContext::DEBUG);
	}

	EXPECT_TRUE(g_memoryAllocatorContext.empty());

	{
		MemAllocJanitor janitor(MemAllocatorContext::DEBUG);
		{
			MemAllocJanitor janitor2(MemAllocatorContext::DEBUG);

			EXPECT_EQ(g_memoryAllocatorContext.top(), MemAllocatorContext::DEBUG);
		}

		EXPECT_EQ(g_memoryAllocatorContext.top(), MemAllocatorContext::DEBUG);
	}

	EXPECT_TRUE(g_memoryAllocatorContext.empty());

	{
		MemAllocJanitor janitor(MemAllocatorContext::DEBUG);
		EXPECT_EQ(g_memoryAllocatorContext.top(), MemAllocatorContext::DEBUG);

		{
			MemAllocJanitor janitor2(MemAllocatorContext::FRAME);
			EXPECT_EQ(g_memoryAllocatorContext.top(), MemAllocatorContext::FRAME);

			{
				MemAllocJanitor janitor3(MemAllocatorContext::STATIC);
				EXPECT_EQ(g_memoryAllocatorContext.top(), MemAllocatorContext::STATIC);

			}
			EXPECT_EQ(g_memoryAllocatorContext.top(), MemAllocatorContext::FRAME);
			EXPECT_FALSE(g_memoryAllocatorContext.empty());

		}

		EXPECT_EQ(g_memoryAllocatorContext.top(), MemAllocatorContext::DEBUG);
		EXPECT_FALSE(g_memoryAllocatorContext.empty());
	}
	
	EXPECT_TRUE(g_memoryAllocatorContext.empty());

	{
		MemAllocJanitor janitor(MemAllocatorContext::DEBUG);
		EXPECT_EQ(g_memoryAllocatorContext.top(), MemAllocatorContext::DEBUG);

		{
			MemAllocJanitor janitor2(MemAllocatorContext::FRAME);
			EXPECT_EQ(g_memoryAllocatorContext.top(), MemAllocatorContext::FRAME);

			{
				MemAllocJanitor janitor3(MemAllocatorContext::STATIC);
				EXPECT_EQ(g_memoryAllocatorContext.top(), MemAllocatorContext::STATIC);

				{
					MemAllocJanitor janitor3(MemAllocatorContext::FRAME);
					EXPECT_EQ(g_memoryAllocatorContext.top(), MemAllocatorContext::FRAME);
				}

				EXPECT_EQ(g_memoryAllocatorContext.top(), MemAllocatorContext::STATIC);
				EXPECT_FALSE(g_memoryAllocatorContext.empty());
			}

			EXPECT_EQ(g_memoryAllocatorContext.top(), MemAllocatorContext::FRAME);
			EXPECT_FALSE(g_memoryAllocatorContext.empty());
		}

		EXPECT_EQ(g_memoryAllocatorContext.top(), MemAllocatorContext::DEBUG);
		EXPECT_FALSE(g_memoryAllocatorContext.empty());
	}

	EXPECT_TRUE(g_memoryAllocatorContext.empty());

}

TEST(StackAllocatorTest, MemAlloc)
{
	{
		MemAllocJanitor janitor(MemAllocatorContext::FRAME);

		TestStruct* p_testStruct = Allocate<TestStruct>();
		EXPECT_NE(p_testStruct, nullptr);

		EXPECT_GT(GetMemoryAllocator(MemAllocatorContext::FRAME)->GetUsedMemorySize(), 0);
		EXPECT_EQ(GetMemoryAllocator(MemAllocatorContext::FRAME)->GetUsedMemorySize(), 16);
	}

	{
		MemAllocJanitor janitor(MemAllocatorContext::FRAME);

		TestStruct* p_testStruct = Allocate<TestStruct>();
		EXPECT_NE(p_testStruct, nullptr);

		EXPECT_GT(GetMemoryAllocator(MemAllocatorContext::FRAME)->GetUsedMemorySize(), 16);
		EXPECT_EQ(GetMemoryAllocator(MemAllocatorContext::FRAME)->GetUsedMemorySize(), 32);
	}

	{
		MemAllocJanitor janitor(MemAllocatorContext::FRAME);

		bool* p_testBool = Allocate<bool>();
		EXPECT_NE(p_testBool, nullptr);

		EXPECT_GT(GetMemoryAllocator(MemAllocatorContext::FRAME)->GetUsedMemorySize(), 32);
		EXPECT_EQ(GetMemoryAllocator(MemAllocatorContext::FRAME)->GetUsedMemorySize(), 33);
	}
	
	EXPECT_EQ(GetMemoryAllocator(MemAllocatorContext::FRAME)->GetAllocFreeRatio(), 3);

	GetMemoryAllocator(MemAllocatorContext::FRAME)->Clear();
}

TEST(StackAllocatorTest, MemAllocAligned)
{
	{
		MemAllocJanitor janitor(MemAllocatorContext::FRAME);

		TestStruct* p_testStruct = AllocAligned<TestStruct>();
		EXPECT_NE(p_testStruct, nullptr);
		
		EXPECT_GT(GetMemoryAllocator(MemAllocatorContext::FRAME)->GetUsedMemorySize(), 16);
		EXPECT_EQ(reinterpret_cast<uint64Ptr>(p_testStruct) % sizeof(TestStruct), 0);
	
		int8* p_testInt = AllocAligned<int8>();
		int16* p_testInt2 = AllocAligned<int16>();
		int32* p_testInt3 = AllocAligned<int32>();
		int64* p_testInt4 = AllocAligned<int64>();

		EXPECT_EQ(reinterpret_cast<uint64Ptr>(p_testInt) % sizeof(int8), 0);
		EXPECT_EQ(reinterpret_cast<uint64Ptr>(p_testInt2) % sizeof(int16), 0);
		EXPECT_EQ(reinterpret_cast<uint64Ptr>(p_testInt3) % sizeof(int32), 0);
		EXPECT_EQ(reinterpret_cast<uint64Ptr>(p_testInt4) % sizeof(int64), 0);
	}

	GetMemoryAllocator(MemAllocatorContext::FRAME)->Clear();
}

TEST(StackAllocatorTest, MemClear)
{
	{
		MemAllocJanitor janitor(MemAllocatorContext::FRAME);

		TestStruct* p_testStruct = AllocAligned<TestStruct>();
		EXPECT_NE(p_testStruct, nullptr);

		EXPECT_GT(GetMemoryAllocator(MemAllocatorContext::FRAME)->GetUsedMemorySize(), 16);
		EXPECT_EQ(reinterpret_cast<uint64Ptr>(p_testStruct) % sizeof(TestStruct), 0);

		int8* p_testInt = AllocAligned<int8>();
		int16* p_testInt2 = AllocAligned<int16>();
		int32* p_testInt3 = AllocAligned<int32>();
		int64* p_testInt4 = AllocAligned<int64>();

		EXPECT_EQ(reinterpret_cast<uint64Ptr>(p_testInt) % sizeof(int8), 0);
		EXPECT_EQ(reinterpret_cast<uint64Ptr>(p_testInt2) % sizeof(int16), 0);
		EXPECT_EQ(reinterpret_cast<uint64Ptr>(p_testInt3) % sizeof(int32), 0);
		EXPECT_EQ(reinterpret_cast<uint64Ptr>(p_testInt4) % sizeof(int64), 0);
	}

	EXPECT_GT(GetMemoryAllocator(MemAllocatorContext::FRAME)->GetUsedMemorySize(), 0);
	EXPECT_LT(GetMemoryAllocator(MemAllocatorContext::FRAME)->GetAvailableMemorySize(), 
		GetMemoryAllocator(MemAllocatorContext::FRAME)->GetTotalMemorySize());
	EXPECT_GT(GetMemoryAllocator(MemAllocatorContext::FRAME)->GetAllocFreeRatio(), 0);

	GetMemoryAllocator(MemAllocatorContext::FRAME)->Clear();

	EXPECT_EQ(GetMemoryAllocator(MemAllocatorContext::FRAME)->GetAllocFreeRatio(), 0);
	EXPECT_EQ(GetMemoryAllocator(MemAllocatorContext::FRAME)->GetAvailableMemorySize(),
		GetMemoryAllocator(MemAllocatorContext::FRAME)->GetTotalMemorySize());
	EXPECT_EQ(GetMemoryAllocator(MemAllocatorContext::FRAME)->GetUsedMemorySize(), 0);
}

TEST(StackAllocatorTest, Full)
{
	StackAllocator stackAlloc = StackAllocator(64);

	RefCntAutoPtr<TestStruct> p_testStruct = stackAlloc.Alloc(sizeof(TestStruct));
	RefCntAutoPtr<TestStruct> p_testStruct1 = stackAlloc.Alloc(sizeof(TestStruct));
	RefCntAutoPtr<TestStruct> p_testStruct2 = stackAlloc.Alloc(sizeof(TestStruct));
	RefCntAutoPtr<TestStruct> p_testStruct3 = stackAlloc.Alloc(sizeof(TestStruct));

	TestStruct* p_testStructFull = nullptr;

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
	
	EXPECT_DEATH(p_testStructFull = new (AccessHandleData(stackAlloc.Alloc(sizeof(TestStruct))->HandleId)) TestStruct(), "");

#endif

	EXPECT_TRUE(p_testStruct.RawPtr() != nullptr);
	EXPECT_TRUE(p_testStruct1.RawPtr() != nullptr);
	EXPECT_TRUE(p_testStruct2.RawPtr() != nullptr);
	EXPECT_TRUE(p_testStruct3.RawPtr() != nullptr);

	EXPECT_TRUE(p_testStructFull == nullptr);
}

TEST(StackAllocatorTest, FullAligned)
{
	StackAllocator stackAlloc = StackAllocator(32);

	auto allocBase = stackAlloc.GetMarker();

	TestStruct* p_testStruct = new (stackAlloc.AllocAligned(sizeof(TestStruct))) TestStruct();
	TestStruct* p_testStructFull = nullptr;

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

	EXPECT_DEATH(p_testStructFull = new (stackAlloc.Alloc(sizeof(TestStruct))) TestStruct(), "");
	
#endif

	EXPECT_TRUE(p_testStruct != nullptr);
	EXPECT_TRUE(reinterpret_cast<uint64Ptr>(allocBase) % sizeof(TestStruct) == 0 || p_testStructFull == nullptr);
}


TEST(PoolAllocatorTest, MemAlloc)
{
	MemAllocJanitor janitor(MemAllocatorContext::DEBUG);

	TestStruct* p_testStruct = Allocate<TestStruct>();

	int8* p_int8 = Allocate<int8>();
	int16* p_int16 = Allocate<int16>();
	int32* p_int32 = Allocate<int32>();
	int64* p_int64 = Allocate<int64>();

	EXPECT_TRUE(p_testStruct != nullptr);
	EXPECT_TRUE(p_int8 != nullptr);
	EXPECT_TRUE(p_int16 != nullptr);
	EXPECT_TRUE(p_int32 != nullptr);
	EXPECT_TRUE(p_int64 != nullptr);

	p_testStruct->myInt = 9;

	*p_int8 = 3;
	*p_int16 = 3847;
	*p_int32 = -83895395;
	*p_int64 = 397283;

	EXPECT_EQ(p_testStruct->myInt, 9);
	EXPECT_EQ(*p_int8, 3);
	EXPECT_EQ(*p_int16, 3847);
	EXPECT_EQ(*p_int32, -83895395);
	EXPECT_EQ(*p_int64, 397283);

	EXPECT_EQ(GetMemoryAllocator(MemAllocatorContext::DEBUG)->GetUsedMemorySize(), 5 * 65536);
	EXPECT_EQ(GetMemoryAllocator(MemAllocatorContext::DEBUG)->GetAllocFreeRatio(), 5);
	EXPECT_GT(GetMemoryAllocator(MemAllocatorContext::DEBUG)->GetAvailableMemorySize(), 0);

	FlushAllocator();
}

TEST(PoolAllocatorTest, MemFree)
{
	PoolAllocator poolAlloc = PoolAllocator(64, 32);

	RefCntAutoPtr<TestStruct> p_testStruct = poolAlloc.Alloc(sizeof(TestStruct));
	EXPECT_EQ(poolAlloc.GetUsedMemorySize(), 32);

	RefCntAutoPtr<int8> p_int8 = poolAlloc.Alloc(sizeof(int8));
	EXPECT_EQ(poolAlloc.GetUsedMemorySize(), 64);

	RefCntAutoPtr<int16> p_int16 = poolAlloc.Alloc(sizeof(int16));
	EXPECT_EQ(poolAlloc.GetUsedMemorySize(), 96);

	RefCntAutoPtr<int32> p_int32 = poolAlloc.Alloc(sizeof(int32));
	EXPECT_EQ(poolAlloc.GetUsedMemorySize(), 128);

	RefCntAutoPtr<int64> p_int64 = poolAlloc.Alloc(sizeof(int64));
	EXPECT_EQ(poolAlloc.GetUsedMemorySize(), 160);


	EXPECT_TRUE(p_testStruct.RawPtr() != nullptr);
	EXPECT_TRUE(p_int8.RawPtr() != nullptr);
	EXPECT_TRUE(p_int16.RawPtr() != nullptr);
	EXPECT_TRUE(p_int32.RawPtr() != nullptr);
	EXPECT_TRUE(p_int64.RawPtr() != nullptr);

	p_testStruct->myInt = 9;

	*p_int8 = 3;
	*p_int16 = 3847;
	*p_int32 = -83895395;
	*p_int64 = 397283;

	EXPECT_EQ(p_testStruct->myInt, 9);
	EXPECT_EQ(*p_int8, 3);
	EXPECT_EQ(*p_int16, 3847);
	EXPECT_EQ(*p_int32, -83895395);
	EXPECT_EQ(*p_int64, 397283);

	EXPECT_EQ(poolAlloc.GetUsedMemorySize(),
		5 * 32);
	EXPECT_EQ(poolAlloc.GetAllocFreeRatio(), 5);
	EXPECT_GT(poolAlloc.GetAvailableMemorySize(), 0);

	poolAlloc.FreeMemory(p_testStruct.RawPtr());
	EXPECT_EQ(poolAlloc.GetUsedMemorySize(), 128);

	poolAlloc.FreeMemory(p_int8.RawPtr());
	EXPECT_EQ(poolAlloc.GetUsedMemorySize(), 96);

	poolAlloc.FreeMemory(p_int16.RawPtr());
	EXPECT_EQ(poolAlloc.GetUsedMemorySize(), 64);

	poolAlloc.FreeMemory(p_int32.RawPtr());
	EXPECT_EQ(poolAlloc.GetUsedMemorySize(), 32);

	poolAlloc.FreeMemory(p_int64.RawPtr());
	EXPECT_EQ(poolAlloc.GetUsedMemorySize(), 0);


	EXPECT_EQ(poolAlloc.GetAllocFreeRatio(), 0);
	EXPECT_EQ(poolAlloc.GetAvailableMemorySize(), poolAlloc.GetTotalMemorySize());
	
#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

	TestStruct* p_deathTest = new TestStruct();
	EXPECT_DEATH(poolAlloc.FreeMemory(p_deathTest), "");
	delete p_deathTest;

#endif

	poolAlloc.Clear();
}

TEST(PoolAllocatorTest, MemAllocAligned)
{
	MemAllocJanitor janitor(MemAllocatorContext::DEBUG);

	TestStruct* p_testStruct = AllocAligned<TestStruct>();

	int8* p_int8 = AllocAligned<int8>();
	int16* p_int16 = AllocAligned<int16>();
	int32* p_int32 = AllocAligned<int32>();
	int64* p_int64 = AllocAligned<int64>();

	EXPECT_TRUE(p_testStruct != nullptr);
	EXPECT_TRUE(p_int8 != nullptr);
	EXPECT_TRUE(p_int16 != nullptr);
	EXPECT_TRUE(p_int32 != nullptr);
	EXPECT_TRUE(p_int64 != nullptr);

	p_testStruct->myInt = 9;

	*p_int8 = 3;
	*p_int16 = 3847;
	*p_int32 = -83895395;
	*p_int64 = 397283;

	EXPECT_EQ(p_testStruct->myInt, 9);
	EXPECT_EQ(*p_int8, 3);
	EXPECT_EQ(*p_int16, 3847);
	EXPECT_EQ(*p_int32, -83895395);
	EXPECT_EQ(*p_int64, 397283);

	EXPECT_EQ(GetMemoryAllocator(MemAllocatorContext::DEBUG)->GetUsedMemorySize(), 5 * 65536);
	EXPECT_EQ(GetMemoryAllocator(MemAllocatorContext::DEBUG)->GetAllocFreeRatio(), 5);
	EXPECT_GT(GetMemoryAllocator(MemAllocatorContext::DEBUG)->GetAvailableMemorySize(), 0);

	EXPECT_EQ(reinterpret_cast<uint64Ptr>(p_testStruct) % sizeof(TestStruct), 0);
	EXPECT_EQ(reinterpret_cast<uint64Ptr>(p_int8) % sizeof(int8), 0);
	EXPECT_EQ(reinterpret_cast<uint64Ptr>(p_int16) % sizeof(int16), 0);
	EXPECT_EQ(reinterpret_cast<uint64Ptr>(p_int32) % sizeof(int32), 0);
	EXPECT_EQ(reinterpret_cast<uint64Ptr>(p_int64) % sizeof(int64), 0);

	FlushAllocator();
}

TEST(PoolAllocatorTest, MemFreeAligned)
{
	MemAllocJanitor janitor(MemAllocatorContext::DEBUG);

	TestStruct* p_testStruct = AllocAligned<TestStruct>();

	int8* p_int8 = AllocAligned<int8>();
	int16* p_int16 = AllocAligned<int16>();
	int32* p_int32 = AllocAligned<int32>();
	int64* p_int64 = AllocAligned<int64>();

	EXPECT_TRUE(p_testStruct != nullptr);
	EXPECT_TRUE(p_int8 != nullptr);
	EXPECT_TRUE(p_int16 != nullptr);
	EXPECT_TRUE(p_int32 != nullptr);
	EXPECT_TRUE(p_int64 != nullptr);

	p_testStruct->myInt = 9;

	*p_int8 = 3;
	*p_int16 = 3847;
	*p_int32 = -83895395;
	*p_int64 = 397283;

	EXPECT_EQ(p_testStruct->myInt, 9);
	EXPECT_EQ(*p_int8, 3);
	EXPECT_EQ(*p_int16, 3847);
	EXPECT_EQ(*p_int32, -83895395);
	EXPECT_EQ(*p_int64, 397283);

	FreeAligned(p_int16);
	FreeAligned(p_int32);
	FreeAligned(p_testStruct);
	FreeAligned(p_int8);
	FreeAligned(p_int64);

	EXPECT_EQ(GetMemoryAllocator(MemAllocatorContext::DEBUG)->GetUsedMemorySize(),0);
	EXPECT_EQ(GetMemoryAllocator(MemAllocatorContext::DEBUG)->GetAllocFreeRatio(), 0);
	EXPECT_GT(GetMemoryAllocator(MemAllocatorContext::DEBUG)->GetAvailableMemorySize(), 0);

	FlushAllocator();
}

TEST(PoolAllocatorTest, Full)
{
	PoolAllocator poolAlloc = PoolAllocator(5, 32);

	TestStruct* p_testStruct = new (poolAlloc.AllocAligned(sizeof(TestStruct))) TestStruct();	// 1
	int32* p_testInt = new (poolAlloc.Alloc(sizeof(int32))) int32();	// 2
	int64* p_testInt2 = new (poolAlloc.Alloc(sizeof(int64))) int64();	// 3
	int16* p_testInt3 = new (poolAlloc.Alloc(sizeof(int16))) int16();	// 4
	int16* p_testInt4 = new (poolAlloc.Alloc(sizeof(int16))) int16();	// 5
	void* p_testInt5 = poolAlloc.Alloc(sizeof(int8));	// 6


	EXPECT_TRUE(p_testStruct != nullptr);
	EXPECT_TRUE(p_testInt != nullptr);
	EXPECT_TRUE(p_testInt2 != nullptr);
	EXPECT_TRUE(p_testInt3 != nullptr);
	EXPECT_TRUE(p_testInt4 != nullptr);

	EXPECT_TRUE(p_testInt5 == nullptr);	// Should be nullptr since pool is full!

	poolAlloc.Clear();
}


TEST(HeapAllocatorTest, MemAlloc)
{
	MemAllocJanitor janitor(MemAllocatorContext::RENDERING);

	TestStruct* p_testStruct = Allocate<TestStruct>();
	EXPECT_NE(p_testStruct, nullptr);

	int64* p_int64 = Allocate<int64>();
	EXPECT_NE(p_int64, nullptr);
	
	FlushAllocator();
}

TEST(HeapAllocatorTest, MemFree)
{
	MemAllocJanitor janitor(MemAllocatorContext::RENDERING);

	TestStruct* p_testStruct = Allocate<TestStruct>();
	EXPECT_NE(p_testStruct, nullptr);

	int64* p_int64 = Allocate<int64>();
	EXPECT_NE(p_int64, nullptr);

	int32* p_int32 = Allocate<int32>();
	EXPECT_NE(p_int32, nullptr);

	p_testStruct->myInt = 3;
	p_testStruct->myBoolArray[0] = true;
	p_testStruct->myBoolArray[1] = false;
	p_testStruct->myBoolArray[2] = true;
	p_testStruct->myBoolArray[3] = false;

	p_testStruct->myFloatArray[0] = 3.14f;
	p_testStruct->myFloatArray[1] = 2.718f;

	*p_int64 = 7;

	Free(p_int64);
	Free(p_int32);
	Free(p_testStruct);

	EXPECT_EQ(GetMemoryAllocator(MemAllocatorContext::RENDERING)->GetUsedMemorySize(), 0);
	EXPECT_EQ(GetMemoryAllocator(MemAllocatorContext::RENDERING)->GetAllocFreeRatio(), 0);
	EXPECT_EQ(GetMemoryAllocator(MemAllocatorContext::RENDERING)->GetAvailableMemorySize(), 
		GetMemoryAllocator(MemAllocatorContext::RENDERING)->GetTotalMemorySize());

	FlushAllocator();
}

TEST(HeapAllocatorTest, MemAllocAligned)
{
	MemAllocJanitor janitor(MemAllocatorContext::RENDERING);

	TestStruct* p_testStruct = AllocAligned<TestStruct>();
	EXPECT_NE(p_testStruct, nullptr);

	int64* p_int64 = AllocAligned<int64>();
	EXPECT_NE(p_int64, nullptr);

	EXPECT_EQ(reinterpret_cast<uint64Ptr>(p_testStruct) % sizeof(TestStruct), 0);
	EXPECT_EQ(reinterpret_cast<uint64Ptr>(p_int64) % sizeof(int64), 0);

	FlushAllocator();
}

TEST(HeapAllocatorTest, MemFreeAligned)
{
	MemAllocJanitor janitor(MemAllocatorContext::RENDERING);

	TestStruct* p_testStruct = AllocAligned<TestStruct>();
	EXPECT_NE(p_testStruct, nullptr);

	int64* p_int64 = AllocAligned<int64>();
	EXPECT_NE(p_int64, nullptr);

	EXPECT_EQ(reinterpret_cast<uint64Ptr>(p_testStruct) % sizeof(TestStruct), 0);
	EXPECT_EQ(reinterpret_cast<uint64Ptr>(p_int64) % sizeof(int64), 0);

	p_testStruct->myInt = 3;
	p_testStruct->myBoolArray[0] = true;
	p_testStruct->myBoolArray[1] = false;
	p_testStruct->myBoolArray[2] = true;
	p_testStruct->myBoolArray[3] = false;

	p_testStruct->myFloatArray[0] = 3.14f;
	p_testStruct->myFloatArray[1] = 2.718f;

	*p_int64 = 7;

	FreeAligned(p_int64);
	FreeAligned(p_testStruct);	// p_testStruct seems to have different mem address???

	EXPECT_EQ(GetMemoryAllocator(MemAllocatorContext::RENDERING)->GetUsedMemorySize(), 0);
	EXPECT_EQ(GetMemoryAllocator(MemAllocatorContext::RENDERING)->GetAllocFreeRatio(), 0);
	EXPECT_EQ(GetMemoryAllocator(MemAllocatorContext::RENDERING)->GetAvailableMemorySize(),
		GetMemoryAllocator(MemAllocatorContext::RENDERING)->GetTotalMemorySize());

	FlushAllocator();
}

TEST(HeapAllocatorTest, Defragment)
{
	HeapAllocator allocTest = HeapAllocator(128);

	RefCntAutoPtr<TestStruct> p_testStruct =		// 16 + 16 bytes
		allocTest.Alloc(sizeof(TestStruct));	// 32
	EXPECT_NE(p_testStruct.RawPtr(), nullptr);

	RefCntAutoPtr<TestStruct> p_testStruct2 =		// 16 + 16 bytes
		allocTest.Alloc(sizeof(TestStruct));	// 32
	EXPECT_NE(p_testStruct2.RawPtr(), nullptr);

	RefCntAutoPtr<TestStruct> p_testStruct3 =		// 16 + 16 bytes
		allocTest.Alloc(sizeof(TestStruct));	// 32

	p_testStruct3->myFloatArray[0] = 3.1415f;

	EXPECT_NE(p_testStruct3.RawPtr(), nullptr);

	// Create a hole
	allocTest.FreeMemory(p_testStruct2.RawPtr());
	EXPECT_EQ(allocTest.GetUsedMemorySize(), 64);

	// Now, although there is enough space, no allocation should be possible
	void* p_testBigStruct = allocTest.Alloc(sizeof(BigStruct));	// 32 + 32 = 64 bytes
	EXPECT_EQ(p_testBigStruct, nullptr);

	//Defragment
	allocTest.Defragment();

	// Now it should work
	void* p_testBigStruct2 = allocTest.Alloc(sizeof(BigStruct));	// 32 + 32 = 64 bytes
	EXPECT_NE(p_testBigStruct2, nullptr);
	EXPECT_FLOAT_EQ(p_testStruct3->myFloatArray[0], 3.1415f);
}

TEST(HeapAllocatorTest, DefragmentAligned)
{
	HeapAllocator allocTest = HeapAllocator(150);

	RefCntAutoPtr<TestStruct> p_testStruct =		// 16 + 16 bytes
		allocTest.Alloc(sizeof(TestStruct));	// 32
	EXPECT_NE(p_testStruct.RawPtr(), nullptr);

	RefCntAutoPtr<TestStruct> p_testStruct2 =		// 16 + 16 bytes
		allocTest.Alloc(sizeof(TestStruct));	// 32
	EXPECT_NE(p_testStruct2.RawPtr(), nullptr);

	RefCntAutoPtr<TestStruct> p_testStruct3 =		// 16 + 16 bytes
		allocTest.AllocAligned(sizeof(TestStruct));	// 32
	EXPECT_NE(p_testStruct3.RawPtr(), nullptr);
	EXPECT_EQ(reinterpret_cast<uint64Ptr>(p_testStruct3.RawPtr()) % sizeof(TestStruct), 0);


	// Create a hole
	allocTest.FreeMemory(p_testStruct2.RawPtr());
	EXPECT_EQ(allocTest.GetUsedMemorySize(), 64);

	// Now, although there is enough space, no allocation should be possible
	void* p_testBigStruct = allocTest.Alloc(sizeof(BigStruct));	// 32 + 32 = 64 bytes
	EXPECT_EQ(p_testBigStruct, nullptr);

	//Defragment
	allocTest.Defragment();

	// Now it should work
	void* p_testBigStruct2 = allocTest.Alloc(sizeof(BigStruct));	// 32 + 32 = 64 bytes
	EXPECT_NE(p_testBigStruct2, nullptr);
	EXPECT_EQ(reinterpret_cast<uint64Ptr>(p_testStruct3.RawPtr()) % sizeof(TestStruct), 0);
}

TEST(HeapAllocatorTest, DefragmentMultiIteration)
{
	HeapAllocator allocTest = HeapAllocator(4096);

	HandleInfo* structArray[11];

	for (int8 i = 0; i < 11; ++i)
	{
		structArray[i] = allocTest.Alloc(sizeof(TestStruct));
		reinterpret_cast<TestStruct*>(AccessHandleData(structArray[i]->HandleId))->myInt = i;
	}

	allocTest.FreeMemory(AccessHandleData(structArray[1]->HandleId));
	allocTest.FreeMemory(AccessHandleData(structArray[3]->HandleId));
	allocTest.FreeMemory(AccessHandleData(structArray[4]->HandleId));
	allocTest.FreeMemory(AccessHandleData(structArray[7]->HandleId));
	allocTest.FreeMemory(AccessHandleData(structArray[9]->HandleId));

	// [ 1 | 0 | 1 | 0 | 0 | 1 | 1 | 0 | 1 | 0 | 1 ]

	EXPECT_NE(AccessHandleData(structArray[0]->HandleId), nullptr);
	EXPECT_NE(AccessHandleData(structArray[2]->HandleId), nullptr);
	EXPECT_NE(AccessHandleData(structArray[5]->HandleId), nullptr);
	EXPECT_NE(AccessHandleData(structArray[6]->HandleId), nullptr);
	EXPECT_NE(AccessHandleData(structArray[8]->HandleId), nullptr);
	EXPECT_NE(AccessHandleData(structArray[10]->HandleId), nullptr);

	void* oldDataPtr0 = AccessHandleData(structArray[0]->HandleId);
	void* oldDataPtr2 = AccessHandleData(structArray[2]->HandleId);
	void* oldDataPtr5 = AccessHandleData(structArray[5]->HandleId);
	void* oldDataPtr6 = AccessHandleData(structArray[6]->HandleId);
	void* oldDataPtr8 = AccessHandleData(structArray[8]->HandleId);
	void* oldDataPtr10 = AccessHandleData(structArray[10]->HandleId);

	allocTest.Defragment(10);

	// Check for nullptr
	EXPECT_NE(AccessHandleData(structArray[0]->HandleId), nullptr);
	EXPECT_NE(AccessHandleData(structArray[2]->HandleId), nullptr);
	EXPECT_NE(AccessHandleData(structArray[5]->HandleId), nullptr);
	EXPECT_NE(AccessHandleData(structArray[6]->HandleId), nullptr);
	EXPECT_NE(AccessHandleData(structArray[8]->HandleId), nullptr);
	EXPECT_NE(AccessHandleData(structArray[10]->HandleId), nullptr);

	// Check if data ptrs changed over the course of the defragmentation 
	// First pointer will not change since it cannot be moved!
	EXPECT_EQ(AccessHandleData(structArray[0]->HandleId), oldDataPtr0);
	EXPECT_NE(AccessHandleData(structArray[2]->HandleId), oldDataPtr2);
	EXPECT_NE(AccessHandleData(structArray[5]->HandleId), oldDataPtr5);
	EXPECT_NE(AccessHandleData(structArray[6]->HandleId), oldDataPtr6);
	EXPECT_NE(AccessHandleData(structArray[8]->HandleId), oldDataPtr8);
	EXPECT_NE(AccessHandleData(structArray[10]->HandleId), oldDataPtr10);

	// Check if data is still valid
	EXPECT_EQ(reinterpret_cast<TestStruct*>(AccessHandleData(structArray[0]->HandleId))->myInt, 0);
	EXPECT_EQ(reinterpret_cast<TestStruct*>(AccessHandleData(structArray[2]->HandleId))->myInt, 2);
	EXPECT_EQ(reinterpret_cast<TestStruct*>(AccessHandleData(structArray[5]->HandleId))->myInt, 5);
	EXPECT_EQ(reinterpret_cast<TestStruct*>(AccessHandleData(structArray[6]->HandleId))->myInt, 6);
	EXPECT_EQ(reinterpret_cast<TestStruct*>(AccessHandleData(structArray[8]->HandleId))->myInt, 8);
	EXPECT_EQ(reinterpret_cast<TestStruct*>(AccessHandleData(structArray[10]->HandleId))->myInt, 10);
}