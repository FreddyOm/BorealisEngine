#pragma once
#include "types.h"
#include "../debug/logger.h"

#if defined(BOREALIS_RELEASE) || defined(BOREALIS_MINSIZEREL)
#include "../math/crc_hash.h"
#include <cstring>
#endif

namespace Borealis::Types
{
	typedef uint64Ptr StringId;

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)

	/// <summary>
	/// Evaluates to a hash value and stores the native string internally.
	/// </summary>
	/// <param name="str">The native string to store.</param>
	/// <returns>The string id.</returns>
	BOREALIS_API extern StringId String(const char* str);

	/// <summary>
	/// Reads the human-readable string value from a StringId.
	/// </summary>
	/// <param name="stringId">The StringId to look up.</param>
	/// <returns>A human readable string with the content stored as that StringId.</returns>
	BOREALIS_API extern const char* ValueFromStringId(StringId stringId);

#else

	/// <summary>
	/// String function directly evaluates to hash value without storing
	/// the readable string inside the internal hash table.
	/// </summary>
	/// <param name="str">The native string to store.</param>
	/// <returns>The string id.</returns>
	BOREALIS_API constexpr StringId String(const char* str)
	{
		Assert(str != nullptr, "Cannot create string from nullptr!");
		Assert(strlen(str) > 0, "Empty strings are not allowed!");

		// TODO: Non-constexpr strlen cannot produce compile-time-static value! Fix me!
		return Math::CompileTimeHashValue(str);
	}

	/// <summary>
	/// String function directly evaluates to an empty string since the string id will not store any 
	/// readable value in release builds.
	/// </summary>
	/// <param name="stringId">The string ID to look up.</param>
	/// <returns>Always an empty string.</returns>
	BOREALIS_API constexpr const char* ValueFromStringId(StringId stringId)
	{
		return "";
	}


#endif
}