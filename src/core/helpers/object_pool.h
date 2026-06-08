#pragma once

#include "../debug/logger.h"
#include "../memory/ref_cnt_auto_ptr.h"
#include "../memory/pool_allocator.h"
#include "../types/types.h"

#include <set>
#include <type_traits>
//#include <concepts>

namespace Borealis::Helpers
{
	template<typename T> concept IsResettable = std::is_base_of_v<Types::IResettable, T>;

	/// <summary>
	/// The object pool can pool objects that implement the IResettable interface. 
	/// This is necessary, so that pool elements can be resetted instead of deconstructed.
	/// </summary>
	/// <typeparam name="T">The type of the object that should be pooled.</typeparam>
	/// <typeparam name="N">The size of the object pool.</typeparam>
	template<typename T, int N> requires IsResettable<T>
	class ObjectPool
	{
	public:

		// TODO: Fix me! The object pool has to default initialize objects! 
		// Therefore, arguments are passed for non-default constructable types for now, but a better solution would be preferrable.
		template<typename ...Args>
		ObjectPool(Args ... args)
		{
			// Reserve all allocated elements inside the inactive elements set
			for (Types::int32 i = 0; i < N; ++i)
			{
				Memory::MemAllocJanitor janitor(Memory::MemAllocatorContext::DEFAULT);
				m_inactiveElements.insert(Memory::RefCntAutoPtr<T>::Allocate(args...));

				//Memory::HandleInfo* pHandleInfo = m_memPool.Alloc(sizeof(T));
				//const T* data = new (Memory::AccessHandleData(pHandleInfo->HandleId)) T();
				//m_inactiveElements.insert( Memory::RefCntAutoPtr<T>(pHandleInfo) );
			}
		}

		~ObjectPool()
		{
			//m_memPool.Clear();
			m_inactiveElements.clear();
			m_activeElements.clear();
		}

		BOREALIS_DELETE_COPY_CONSTRUCT(ObjectPool)
		BOREALIS_DELETE_MOVE_CONSTRUCT(ObjectPool)
		BOREALIS_DELETE_COPY_ASSIGN(ObjectPool)
		BOREALIS_DELETE_MOVE_ASSIGN(ObjectPool)

		template<typename ...Args>
		Memory::RefCntAutoPtr<T> Get(Args ... args)
		{
			if (!m_inactiveElements.empty())
			{
				// Copy construct "new" object
				Memory::RefCntAutoPtr<T> obj (*m_inactiveElements.begin());
				void* targetPtr = obj.RawPtr();
				m_inactiveElements.erase(m_inactiveElements.begin());


				new (obj.RawPtr()) T(args...);	// Init "new" object 
				m_activeElements.insert(obj);

				return obj;
			}

			LogError("Unable to allocate more objects in the pool. Returning nullptr.");
			return nullptr;
		}

		/// <summary>
		/// Frees a pool element from the pool (returns it back to the available elements).
		/// </summary>
		/// <param name="pData">A pointer to the element.</param>
		void Return(Memory::RefCntAutoPtr<T> data)
		{
			if (!m_activeElements.empty() && m_activeElements.find(data) != m_activeElements.end())
			{
				dynamic_cast<Types::IResettable*>(data.RawPtr())->Reset();
				m_activeElements.erase(data);
				m_inactiveElements.insert(data);
				return;
			}

			LogError("The requested data is not available in the active pool elements or there are no active elements at all.");
			return;
		}

		// Returns a pointer
		std::set<Memory::RefCntAutoPtr<T>>& GetActiveElements()
		{
			return m_activeElements;
		}

	private:

		//Borealis::Memory::PoolAllocator m_memPool = Borealis::Memory::PoolAllocator(N, sizeof(T));
		std::set<Memory::RefCntAutoPtr<T>> m_activeElements{};
		std::set<Memory::RefCntAutoPtr<T>> m_inactiveElements{};
	};
}