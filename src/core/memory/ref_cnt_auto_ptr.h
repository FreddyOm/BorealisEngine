#pragma once

#include "../types/types.h"
#include "../debug/logger.h"
#include "./memory.h"
#include <typeinfo>

namespace Borealis::Memory
{
	template<typename T>
	struct RefCntAutoPtr
	{
		RefCntAutoPtr()
		{ }
		
		RefCntAutoPtr(HandleInfo* const p_hndlInfo)
		{
			Assert(p_hndlInfo != nullptr, 
				"Cannot assign a nullptr to a ref counted auto pointer object.");

			Assert(this->m_pHandleInfo == nullptr,
				"Cannot assign a raw pointer to a RefCntAutoPtr that is already referencing some data!");
			
			this->m_pHandleInfo = p_hndlInfo;
		}

		RefCntAutoPtr(const RefCntAutoPtr<T>& other)
		{
			Assert(other.m_pHandleInfo != m_pHandleInfo, 
				"Cannot assign a ref counted pointer to itself!");

			// Assign the appropriate data
			m_pHandleInfo = other.m_pHandleInfo;

			// Increment RefCount
			++m_pHandleInfo->RefCount;
		}

		RefCntAutoPtr(RefCntAutoPtr<T>&& other) noexcept
		{
			Assert(other.m_pHandleInfo != m_pHandleInfo && *this != other,
				"Cannot assign a ref counted pointer to itself!");

			// Assign the appropriate data
			m_pHandleInfo = other.m_pHandleInfo;

			// Do not increment RefCount since data is moved!
			other.m_pHandleInfo = nullptr;
		}

		~RefCntAutoPtr()
		{
			// Decrease ref count if possible
			if (m_pHandleInfo != nullptr)
			{
				if (--m_pHandleInfo->RefCount <= 0)
				{
					// Release all memory and clean up!
					RemoveHandle(m_pHandleInfo->HandleId, m_pHandleInfo);
				}
			}
		}

		RefCntAutoPtr& operator=(HandleInfo* const p_hndlInfo)
		{
			Assert(p_hndlInfo != nullptr,
				"Cannot assign a nullptr to a ref counted auto pointer object.");

			Assert(this->m_pHandleInfo == nullptr, 
				"Cannot assign a raw pointer to a RefCntAutoPtr that is already referencing some data!");

			this->m_pHandleInfo = p_hndlInfo;
			return *this;
		}

		RefCntAutoPtr& operator=(const RefCntAutoPtr<T>& other)
		{
			Assert(other.m_pHandleInfo != m_pHandleInfo,
				"Cannot assign a ref counted pointer to itself!");

			// Assign the appropriate data
			m_pHandleInfo = other.m_pHandleInfo;

			// Increment RefCount
			++m_pHandleInfo->RefCount;

			return *this;
		}

		RefCntAutoPtr& operator=(RefCntAutoPtr<T>&& other) noexcept
		{
			Assert(other.m_pHandleInfo != m_pHandleInfo && *this != other,
				"Cannot assign a ref counted pointer to itself!");

			// Assign the appropriate data
			m_pHandleInfo = other.m_pHandleInfo;

			// Do not increment RefCount since data is moved!

			other.m_pHandleInfo = nullptr;

			return *this;
		}

		bool operator<(const RefCntAutoPtr<T>& other) const noexcept
		{
			return m_pHandleInfo->HandleId < other.m_pHandleInfo->HandleId;
		}

		const bool IsValid() const
		{
			return m_pHandleInfo != nullptr;
		}

		template<typename ...Args>
		static HandleInfo* Allocate(Args ... args)
		{
			if (g_memoryAllocatorContext.empty())
			{
				LogError("No memory allocator assigned for allocation! Use a MemAllocJanitor to push an allocator context!");
				return nullptr;
			}

			HandleInfo* p_hndl = GetMemoryAllocator(g_memoryAllocatorContext.top())->Alloc(sizeof(T));
			if (p_hndl) 
			{	
				new (AccessHandleData(p_hndl->HandleId)) T(args...);
				return p_hndl;
			}

			LogError("Couldn't allocate memory!");
			return nullptr;
		}

		static HandleInfo* AllocBlock(Types::uint64 blockSize)
		{
			if (g_memoryAllocatorContext.empty())
			{
				LogError("No memory allocator assigned for allocation! Use a MemAllocJanitor to push an allocator context!");
				return nullptr;
			}

			HandleInfo* p_hndl = GetMemoryAllocator(g_memoryAllocatorContext.top())->Alloc(blockSize);
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

		/// <summary>
		/// Casts a type stored in a RefCntAutoPtr into a derived type or the other way around.
		/// </summary>
		/// <typeparam name="S"></typeparam>
		/// <param name="ref"></param>
		/// <returns></returns>
		template<typename S>
		static RefCntAutoPtr<S> DynamicCastTo(RefCntAutoPtr<T>& ref)
		{
			// TODO: Is this just a check if it works? Could I just return RefCntAutoPtr<S>(ref.m_pHandleInfo)?
			S* casted = dynamic_cast<S*>(ref.RawPtr());
			Assert(casted != nullptr, "Failed to dynamic cast RefCntAutoPtr from \"%s\" to \"%s\"!", typeid(T).name(), typeid(S).name());
			
			// Here we have to increment the ref count manually since we create a new RefCntAutoPtr out of a handleInfo which will not increase the ref cnt.
			// But since the dynamic cast takes a RefCntAutoPtr and returns one as well, this is basically a copy!
			++ref.m_pHandleInfo->RefCount;

			return RefCntAutoPtr<S>(ref.m_pHandleInfo);
		}

		template<typename S>
		static RefCntAutoPtr<S> StaticCastTo(RefCntAutoPtr<T>& ref)
		{
			// TODO: Is this just a check if it works? Could I just return RefCntAutoPtr<S>(ref.m_pHandleInfo)?
			S* casted = static_cast<S*>(ref.RawPtr());
			Assert(casted != nullptr, "Failed to static cast RefCntAutoPtr from \"%s\" to \"%s\"!", typeid(T).name(), typeid(S).name());

			// Here we have to increment the ref count manually since we create a new RefCntAutoPtr out of a handleInfo which will not increase the ref cnt.
			// But since the static cast takes a RefCntAutoPtr and returns one as well, this is basically a copy!
			++ref.m_pHandleInfo->RefCount;

			return RefCntAutoPtr<S>(ref.m_pHandleInfo);
		}

		bool operator==(const RefCntAutoPtr<T> &other) const noexcept
		{
			// Two RefCntAutoPtr instances are the same if they point to the same handle info
			return m_pHandleInfo != nullptr 
				&& other.m_pHandleInfo != nullptr 
				&& m_pHandleInfo == other.m_pHandleInfo;
		}

		/// <summary>
		/// Safely accesses the member of the underlying object. 
		/// Data moved by memory defragmentation will be correctly referenced.
		/// </summary>
		/// <returns>If the reference is valid, the underlying data is returned.
		/// If the reference was invalidated, a nullptr is returned.</returns>
		T* operator->() const
		{
			Assert(m_pHandleInfo != nullptr, 
				"The RefCntAutoPtr has no handle info! Make sure the instance has been set up correctly using \"Allocate()\" or by assigning another valid instance!");
			
			return reinterpret_cast<T*>(AccessHandleData(m_pHandleInfo->HandleId));
		}
		
		/// <summary>
		/// Safely derefernces the member of the underlying object. 
		/// Data moved by memory defragmentation will be correctly referenced.
		/// </summary>
		/// <returns>If the reference is valid, the underlying data is returned.
		/// If the reference was invalidated, a nullptr is returned.</returns>
		T* RawPtr() const
		{
			if (m_pHandleInfo == nullptr)
			{
				LogError("The RefCntAutoPtr has no handle info! Make sure the instance has been set up correctly using \"Allocate()\" or by assigning another valid instance!");
				return nullptr;
			}
			
			return reinterpret_cast<T*>(AccessHandleData(m_pHandleInfo->HandleId));
		}

		/// <summary>
		/// Dereferences the RefCntAutoPtr and returns the underlying object. Asserts if no object is referenced!
		/// </summary>
		/// <returns>A reference to the underlying object.</returns>
		T& operator*() const
		{
			T* p_Data = RawPtr();
			
			Assert(p_Data != nullptr,
				"The RefCntAutoPtr is invalid! Make sure the instance has been set up correctly using \"Allocate()\" or by assigning another valid instance!");
			
			return reinterpret_cast<T&>(*p_Data);
		}

		const Types::int16 UseCount() const
		{
			if (m_pHandleInfo == nullptr)
			{
				LogError("The RefCntAutoPtr has no handle info! Make sure the instance has been set up correctly using \"Allocate()\" or by assigning another valid instance!");
				return 0;
			}

			return m_pHandleInfo->RefCount;
		}
		
	private:

		HandleInfo* m_pHandleInfo = nullptr;
	};
}

namespace std {
	template<typename T>
	struct hash<Borealis::Memory::RefCntAutoPtr<T>> {
		size_t operator()(const Borealis::Memory::RefCntAutoPtr<T>& ptr) const noexcept {
			// Hash basierend auf der Adresse des verwalteten Objekts
			return hash<T*>()(ptr.RawPtr());
		}
	};
}