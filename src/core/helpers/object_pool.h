#pragma once

#include "../debug/logger.h"
#include "../memory/pool_allocator.h"
#include "../types/types.h"

#include <set>
#include <type_traits>
#include <concepts>

namespace Borealis::Helpers
{
	template<typename T> concept IsResettable = std::is_base_of_v<Types::IResettable, T>;

	template<typename T, int N> requires IsResettable<T>
	class ObjectPool
	{
	public:

		ObjectPool() :
			alloc(new Borealis::Memory::PoolAllocator(N, sizeof(T)))
		{
			// Reserve all allocated elements inside the inactive elements set
			for (Types::int32 i = 0; i < N; ++i)
			{
				inactiveElements.insert( reinterpret_cast<T*>( alloc->RawAlloc( sizeof(T) ) ) );
			}
		}

		~ObjectPool()
		{
			alloc->Clear();
		}

		ObjectPool(const ObjectPool& other) = delete;
		ObjectPool(ObjectPool&& other) noexcept = delete;

		void operator=(const ObjectPool& other) = delete;
		void operator=(ObjectPool&& other) noexcept = delete;

		template<typename ...Args>
		T* Get(Args ... args)
		{
			if (!inactiveElements.empty())
			{
				// Construct object
				T* obj = *inactiveElements.begin();
				inactiveElements.erase(inactiveElements.begin());

				T* initObj = new (obj) T(args...);
				activeElements.insert(initObj);

				return initObj;
			}

			LogError("Unable to allocate more objects in the pool. Returning nullptr.");
			return nullptr;
		}

		/// <summary>
		/// Frees a pool element from the pool (returns it back to the available elements).
		/// </summary>
		/// <param name="pData">A pointer to the element.</param>
		void Return(T* pData)
		{
			if (!activeElements.empty() && activeElements.find(pData) != activeElements.end())
			{
				dynamic_cast<Types::IResettable*>(pData)->Reset();
				activeElements.erase(pData);
				inactiveElements.insert(pData);
				return;
			}

			LogError("The requested data is not available in the active pool elements or there are no active elements at all.");
			return;
		}

		// Returns a pointer
		std::set<T*>& GetActiveElements()
		{
			return activeElements;
		}

	private:

		Borealis::Memory::PoolAllocator* const alloc = nullptr;
		std::set<T*> activeElements{};
		std::set<T*> inactiveElements{};
	};
}