#pragma once
#include "allocator.h"
#include "../../config.h"
#include "../types/types.h"
#include <list>
#include <vector>

#ifndef CLEAR_HEAP_ELEMENT_ON_FREE
#define CLEAR_HEAP_ELEMENT_ON_FREE
#endif

namespace Borealis::Memory
{

	//Heap [ HeapDesc | Alignment | Alignment | 1 | 1 | 1 | 1 | 0 | 0 | 0 | .. | HeapDesc | 1 | 1 | 1 ]
	struct HeapDescription	// 16 bytes
	{
		HeapDescription(void* p_data, const Types::uint16 size, const Types::uint16 alignOffset = 0)
			: p_Data(p_data), Size(size), AlignOffset(alignOffset)
		{ }

		void* p_Data = nullptr;				// 8 bytes
		Types::uint16 Size = 0;				// 2 bytes
		Types::uint16 AlignOffset = 0;		// 2 bytes
		Types::uint8 Padding[4] = {};		// 4 bytes
		

		inline Types::uint64Ptr GetMemBlockStart() const
		{
			return reinterpret_cast<Types::uint64Ptr>(p_Data) - AlignOffset - sizeof(HeapDescription);
		}

		inline Types::uint64Ptr GetMemBlockEnd() const
		{
			return reinterpret_cast<Types::uint64Ptr>(p_Data) + Size;
		}
	};
	
	struct HeapFreeListEntry	// 16 bytes
	{
		HeapFreeListEntry(void* p_blockStart, void* p_blockEnd)
			: p_BlockStart(p_blockStart), p_BlockEnd(p_blockEnd)
		{ }

		// The start address of the free memory block
		void* p_BlockStart;		// 8 bytes
		void* p_BlockEnd;		// 8 bytes

		/// <summary>
		/// Get the available size in the memory block
		/// </summary>
		/// <returns>The available block size</returns>
		inline Types::int32 AvailableSize() const
		{
			return reinterpret_cast<Types::uint64Ptr>(p_BlockEnd) - reinterpret_cast<Types::uint64Ptr>(p_BlockStart);
		}
		
		/// <summary>
		/// Splits this memory block into two, and returns the first one for seperate use.
		/// </summary>
		/// <param name="blockSize">The requested block size.</param>
		/// <param name="alignOffset">The alignment size if necessary.</param>
		/// <returns>A new heap description object referencing the newly split memory block.</returns>
		void* AllocMemoryFromBlock(const Types::int32 blockSize, const bool alignToSize = false);
	};

	class BOREALIS_API HeapAllocator : public IMemoryAllocator
	{
	public:

		HeapAllocator();
		HeapAllocator(Types::uint64 size);
		HeapAllocator(const HeapAllocator& other) = delete;
		HeapAllocator(HeapAllocator&& other) noexcept;

		~HeapAllocator() override;

		HeapAllocator& operator=(const HeapAllocator& other) = delete;
		HeapAllocator& operator=(HeapAllocator&& other) noexcept;

	public:

		// Inherited via IMemoryAllocator
		void* Alloc(const Types::uint16 allocSize) override;
		void* AllocAligned(const Types::uint16 allocSize) override;

		void FreeMemory(const void* address) override;
		void FreeAligned(const void* address) override;

		void Clear() override;

		/// <summary>
		/// Defragments the heap by moving memory blocks from higher addresses to lower ones.
		/// </summary>
		/// <param name="iterations">The number of blocks to move per function call. 
		/// Use this to adjust the computational load when defragmenting memory over multiple frames.</param>
		void Defragment(const Types::uint8 iterations = 8);

		Borealis::Types::uint64 GetTotalMemorySize() const override;
		Borealis::Types::uint64 GetUsedMemorySize() const override;
		Borealis::Types::uint64 GetAvailableMemorySize() const override;
		Borealis::Types::int8 GetAllocFreeRatio() const override;

	private:
		bool ReturnFreeBlock(std::list<HeapFreeListEntry>::iterator it, const HeapDescription* const p_desc);
		void MergeFreeBlocks(std::list<HeapFreeListEntry>::iterator const first, 
			std::list<HeapFreeListEntry>::iterator const second);
		
	private:

		Types::uint64Ptr heapBasePtr = 0;

		// @TODO:  Keep an eye on this, this might be slow due to native mem alloc
		std::list<HeapFreeListEntry>* p_freeMemBlockList = nullptr;
	};
}