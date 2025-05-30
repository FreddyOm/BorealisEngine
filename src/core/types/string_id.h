#pragma once
#include "../../config.h"
#include "types.h"

#if defined(BOREALIS_RELEASE) || defined(BOREALIS_MINSIZEREL)
#include "../math/crc_hash.h"
#endif

namespace Borealis::Types
{
	typedef uint64 StringId;

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
	BOREALIS_API extern Types::uint64Ptr InternString(const char* str);

	/// <summary>
	/// Evaluates to a hash value and stores the native string internally.
	/// </summary>
	/// <param name="str">The native string to store.</param>
	/// <returns>The string id.</returns>
	StringId String(const char* str)
	{
		return InternString(str);
	}

#else

	/// <summary>
	/// String function directly evaluates to hash value without storing
	/// the readable string inside the internal hash table.
	/// </summary>
	/// <param name="str">The native string to store.</param>
	/// <returns>The string id.</returns>
	BOREALIS_API constexpr StringId String(const char* str)
	{
		return Math::CompileTimeHashValue(str);
	}

#endif
}