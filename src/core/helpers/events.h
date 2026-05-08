#pragma once
#include "../debug/logger.h"
#include "macros.h"
#include <functional>
#include <vector>
#include <type_traits>

namespace Borealis::Events
{
	template<typename A, typename R = void>
	class Action
	{
	public:
	
		Action()
		{
			m_Callbacks.reserve(8);
		}

		~Action()
		{
			// Clear all registered callbacks
			UnsubscribeAll();
		}

		BOREALIS_DELETE_COPY_CONSTRUCT(Action)
		BOREALIS_DELETE_MOVE_CONSTRUCT(Action)
		BOREALIS_DELETE_COPY_ASSIGN(Action)
		BOREALIS_DELETE_MOVE_ASSIGN(Action)

		void Subscribe(const std::function<R(A)>& callback)
		{
			Assert(callback != nullptr, "Cannot add a nullptr function as an action.");
			m_Callbacks.emplace_back(callback);
		}

		void UnsubscribeAll()
		{
			m_Callbacks.clear();
		}
		
		bool HasCallbacks() const
		{
			return !m_Callbacks.empty();
		}

		// Invoke for 1-argument actions
		template<typename T = A>
		std::enable_if_t<!std::is_void_v<T>, void>
			Invoke(T args)
		{
			for (const auto& callback : m_Callbacks)
			{
				callback(args);
			}
		}

		// Invoke for 0-argument actions
		template<typename T = A>
		std::enable_if_t<std::is_void_v<T>, void>
			Invoke()
		{
			for (const auto& callback : m_Callbacks)
			{
				callback();
			}
		}

		explicit operator bool() const noexcept
		{
			return HasCallbacks();
		}

	private:

		std::vector<std::function<R(A)>> m_Callbacks{};
	};
}