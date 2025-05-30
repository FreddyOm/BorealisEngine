#pragma once

#include "../../config.h"
#include "../types/types.h"
#include "../debug/logger.h"
#include "./memory.h"

namespace Borealis::Memory
{
	class string;

	template<typename T>
	struct RefCntAutoPtr
	{
		RefCntAutoPtr() = delete;

		RefCntAutoPtr<T>(HandleInfo* const p_hndlInfo)
		{
			Assert(p_hndlInfo != nullptr, 
				"Cannot assign a nullptr to a ref counted auto pointer object.");

			Assert(this->p_handleInfo == nullptr,
				"Cannot assign a raw pointer to a RefCntAutoPtr that is already referencing some data!");
			
			this->p_handleInfo = p_hndlInfo;
		}

		RefCntAutoPtr<T>(const RefCntAutoPtr<T>& other)
		{
			Assert(other.p_handleInfo != p_handleInfo, 
				"Cannot assign a ref counted pointer to itself!");

			// Assign the appropriate data
			p_handleInfo = other.p_handleInfo;

			// Increment RefCount
			++p_handleInfo->RefCount;
		}

		RefCntAutoPtr<T>(RefCntAutoPtr<T>&& other) noexcept
		{
			Assert(other.p_handleInfo != p_handleInfo && *this != other,
				"Cannot assign a ref counted pointer to itself!");

			// Assign the appropriate data
			p_handleInfo = other.p_handleInfo;

			// Do not increment RefCount since data is moved!
			other.p_handleInfo = nullptr;
		}

		~RefCntAutoPtr()
		{
			// Decrease ref count if possible
			if (p_handleInfo != nullptr)
			{
				if (--p_handleInfo->RefCount <= 0)
				{
					// Release all memory and clean up!
					RemoveHandle(p_handleInfo->HandleId, p_handleInfo);
				}
			}
		}

		RefCntAutoPtr<T>& operator=(HandleInfo* const p_hndlInfo)
		{
			Assert(p_hndlInfo != nullptr,
				"Cannot assign a nullptr to a ref counted auto pointer object.");

			Assert(this->p_handleInfo == nullptr, 
				"Cannot assign a raw pointer to a RefCntAutoPtr that is already referencing some data!");

			this->p_handleInfo = p_hndlInfo;
			return *this;
		}

		RefCntAutoPtr<T>& operator=(const RefCntAutoPtr<T>& other)
		{
			Assert(other.p_handleInfo != p_handleInfo,
				"Cannot assign a ref counted pointer to itself!");

			// Assign the appropriate data
			p_handleInfo = other.p_handleInfo;

			// Increment RefCount
			++p_handleInfo->RefCount;

			return *this;
		}

		RefCntAutoPtr<T>& operator=(RefCntAutoPtr<T>&& other) noexcept
		{
			Assert(other.p_handleInfo != p_handleInfo && *this != other,
				"Cannot assign a ref counted pointer to itself!");

			// Assign the appropriate data
			p_handleInfo = other.p_handleInfo;

			// Do not increment RefCount since data is moved!

			other.p_handleInfo = nullptr;

			return *this;
		}

		static HandleInfo* Allocate()
		{
			if (g_memoryAllocatorContext.empty())
			{
				LogError("No memory allocator assigned for allocation! Use a MemAllocJanitor to push an allocator context!");
				return nullptr;
			}

			HandleInfo* p_hndl = GetMemoryAllocator(g_memoryAllocatorContext.top())->Alloc(sizeof(T));
			if (p_hndl) 
			{	
				new (AccessHandleData(p_hndl->HandleId)) T();
				return p_hndl;
			}

			LogError("Couldn't allocate memory!");
			return nullptr;
		}

		static HandleInfo* AllocAligned()
		{
			if (g_memoryAllocatorContext.empty())
			{
				LogError("No memory allocator assigned for allocation! Use a MemAllocJanitor to push an allocator context!");
				return nullptr;
			}

			HandleInfo* p_hdnl = GetMemoryAllocator(g_memoryAllocatorContext.top())->AllocAligned(sizeof(T));
			if (p_hdnl)
			{
				new (AccessHandleData(p_hdnl->HandleId)) T();
				return p_hdnl;
			}

			LogError("Couldn't allocate memory!");
			return nullptr;
		}

		bool operator==(const RefCntAutoPtr<T> &other) const
		{
			// Two RefCntAutoPtr instances are the same if they point to the same handle info
			return p_handleInfo != nullptr 
				&& other.p_handleInfo != nullptr 
				&& p_handleInfo == other.p_handleInfo;
		}
		
		T* operator->() const
		{
			Assert(p_handleInfo != nullptr, 
				"The RefCntAutoPtr has no handle info! Make sure the instance has been set up correctly using \"Allocate()\" or by assigning another valid instance!");
			
			return reinterpret_cast<T*>(AccessHandleData(p_handleInfo->HandleId));
		}
		
		T* RawPtr() const
		{
			Assert(p_handleInfo != nullptr,
				"The RefCntAutoPtr has no handle info! Make sure the instance has been set up correctly using \"Allocate()\" or by assigning another valid instance!");

			return reinterpret_cast<T*>(AccessHandleData(p_handleInfo->HandleId));
		}

		T& operator*()
		{
			T* p_Data = RawPtr();
			return reinterpret_cast<T&>(*p_Data);
		}

		Types::int16 UseCount() const
		{
			Assert(p_handleInfo != nullptr,
				"The RefCntAutoPtr has no handle info! Make sure the instance has been set up correctly using \"Allocate()\" or by assigning another valid instance!");

			return p_handleInfo->RefCount;
		}
		
	private:

		HandleInfo* p_handleInfo = nullptr;
	};
}