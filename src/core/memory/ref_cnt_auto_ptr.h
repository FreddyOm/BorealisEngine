#pragma once

#include "../../config.h"
#include "../types/types.h"
#include "../debug/logger.h"
#include "pool_allocator.h"
#include <memory>

#define NDEBUG

namespace Borealis::Memory
{
	class string;

	extern BOREALIS_API PoolAllocator RefCntInfoAllocator;

	/// <summary>
	/// An object to describe the pointers state.
	/// </summary>
	enum class REF_CNT_AUTO_PTR_STATE : Types::int8
	{
		// The pointer is invalid
		INVALID = -1,

		// The pointer is valid
		VALID = 0,

		// The pointer's memory has been released
		INVALID_MEM_FREE = 1,

		// The pointer has been moved to another instance
		INVALID_MOVED = 2,
	};

#ifdef NDEBUG
	BOREALIS_API std::string GetPointerInvalidReason(const REF_CNT_AUTO_PTR_STATE _state);
#endif
	
	struct RefCntAutoPtrInfo
	{
		Types::int64 RefCount = -1;
		REF_CNT_AUTO_PTR_STATE State = REF_CNT_AUTO_PTR_STATE::INVALID;
	};

	template<typename T>
	struct RefCntAutoPtr
	{
		RefCntAutoPtr<T>(T* other)
		{
			Debug::Assert(other != nullptr, 
				"Cannot assign a nullptr to a ref counted auto pointer object.");

			Debug::Assert(p_refCntInfo == nullptr,
				"Cannot assign a raw pointer to a RefCntAutoPtr that is already referencing some data!");
			
			this->p_data = other;

			// Allocate new ref cnt ptr info and write to it!
			this->p_refCntInfo = reinterpret_cast<RefCntAutoPtrInfo*>( RefCntInfoAllocator.Alloc(sizeof(RefCntAutoPtrInfo)));
			this->p_refCntInfo->RefCount = 1;
			this->p_refCntInfo->State = REF_CNT_AUTO_PTR_STATE::VALID;

#ifdef NDEBUG
			this->state = REF_CNT_AUTO_PTR_STATE::VALID;
#endif
		}

		RefCntAutoPtr<T>(const RefCntAutoPtr<T>& other)
		{
			Debug::Assert(other.p_data != p_data, 
				"Cannot assign a ref counted pointer to itself!");

			// Assign the appropriate data
			p_data = other.p_data;
			p_refCntInfo = other.p_refCntInfo;

			// Increment RefCount
			++p_refCntInfo->RefCount;

#ifdef NDEBUG
			this->state = REF_CNT_AUTO_PTR_STATE::VALID;
#endif
		}

		RefCntAutoPtr<T>(RefCntAutoPtr<T>&& other) noexcept
		{
			Debug::Assert(other.p_data != p_data && *this != other,
				"Cannot assign a ref counted pointer to itself!");

			// Assign the appropriate data
			p_data = other.p_data;
			p_refCntInfo = other.p_refCntInfo;

			// Do not increment RefCount since data is moved!

			other.p_data = nullptr;
			other.p_refCntInfo = nullptr;

#ifdef NDEBUG
			this->state = REF_CNT_AUTO_PTR_STATE::VALID;
			other.state = REF_CNT_AUTO_PTR_STATE::INVALID_MOVED;
#endif
		}

		~RefCntAutoPtr<T>()
		{
			// Decrease ref count if possible
			if (p_refCntInfo != nullptr)
			{
				--p_refCntInfo->RefCount;
			
				if (p_refCntInfo->RefCount <= 0 && 
					this->state != REF_CNT_AUTO_PTR_STATE::INVALID)
				{
					// Release all memory and clean up!
					RefCntInfoAllocator.FreeMemory(p_refCntInfo);
#ifdef NDEBUG
					this->state = REF_CNT_AUTO_PTR_STATE::INVALID;
#endif
				}
			}
		}
		
		RefCntAutoPtr<T>& operator=(const T* other)
		{
			Debug::Assert(p_data != nullptr,
				"Cannot assign a nullptr to a ref counted auto pointer object.");

			Debug::Assert(p_refCntInfo == nullptr, 
				"Cannot assign a raw pointer to a RefCntAutoPtr that is already referencing some data!");
		
			this->p_data = other;

			// Allocate new ref cnt ptr info and write to it!
			this->p_refCntInfo = reinterpret_cast<RefCntAutoPtrInfo>(RefCntInfoAllocator.Alloc(sizeof(RefCntAutoPtrInfo)));
			this->p_refCntInfo.RefCount = 1;
			this->p_refCntInfo.State = REF_CNT_AUTO_PTR_STATE::VALID;

#ifdef NDEBUG
			this->state = REF_CNT_AUTO_PTR_STATE::VALID;
#endif
			return *this;
		}

		RefCntAutoPtr<T>& operator=(const RefCntAutoPtr<T>& other)
		{
			Debug::Assert(other.p_data != p_data,
				"Cannot assign a ref counted pointer to itself!");

			// Assign the appropriate data
			p_data = other.p_data;
			p_refCntInfo = other.p_refCntInfo;

			// Increment RefCount
			++p_refCntInfo->RefCount;

#ifdef NDEBUG
			this->state = REF_CNT_AUTO_PTR_STATE::VALID;
#endif
			return *this;
		}

		RefCntAutoPtr<T>& operator=(RefCntAutoPtr<T>&& other) noexcept
		{
			Debug::Assert(other.p_data != p_data && *this != other,
				"Cannot assign a ref counted pointer to itself!");

			// Assign the appropriate data
			p_data = other.p_data;
			p_refCntInfo = other.p_refCntInfo;

			// Do not increment RefCount since data is moved!

			other.p_data = nullptr;
			other.p_refCntInfo = nullptr;

#ifdef NDEBUG
			this->state = REF_CNT_AUTO_PTR_STATE::VALID;
			other.state = REF_CNT_AUTO_PTR_STATE::INVALID_MOVED;
#endif
			return *this;
		}

		bool operator==(const RefCntAutoPtr<T> &other) const
		{
#ifdef NDEBUG
			if (state != REF_CNT_AUTO_PTR_STATE::VALID)
			{
				Debug::LogError(GetPointerInvalidReason(state).c_str());
				return false;
			}
			else if (other.state != REF_CNT_AUTO_PTR_STATE::VALID)
			{
				Debug::LogError(GetPointerInvalidReason(other.state).c_str());
				return false;
			}
#endif
			return p_data != nullptr && other.p_data != nullptr && p_data == other.p_data;
		}
		
		T* operator->() const
		{
#ifdef NDEBUG
			if (state != REF_CNT_AUTO_PTR_STATE::VALID)
			{
				Debug::LogError(GetPointerInvalidReason(state).c_str());
				Debug::Assert(false, "Terminating program due to error. See the last error message for further information.");
			}
#endif
			return p_data;
		}

		T* operator&() const
		{
#ifdef NDEBUG
			if (state != REF_CNT_AUTO_PTR_STATE::VALID)
			{
				Debug::LogError(GetPointerInvalidReason(state).c_str());
			}
#endif
			return p_data;
		}

		T* RawPtr() const
		{
#ifdef NDEBUG
			if (state != REF_CNT_AUTO_PTR_STATE::VALID)
			{
				Debug::LogError(GetPointerInvalidReason(state).c_str());
				return nullptr;
			}
#endif
			return p_data;
		}

		Types::int16 UseCount() const
		{
			return p_refCntInfo != nullptr ? p_refCntInfo->RefCount : -1;
		}

	private:

		T* p_data = nullptr;
		RefCntAutoPtrInfo* p_refCntInfo = nullptr;

#ifdef NDEBUG
		REF_CNT_AUTO_PTR_STATE state = REF_CNT_AUTO_PTR_STATE::INVALID;
#endif
	};
}