#include "string_id.h"

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
#include "../math/crc_hash.h"

#include <unordered_map>
#include <cstring>

namespace Borealis::Types
{
	// TODO: Implement custom hash table that allocates memory from pool allocator!
	std::unordered_map<Types::StringId, const char*> g_StringIdTable;

	Types::StringId InternString(const char* str)
	{
		Assert(str != nullptr, "Cannot create string from nullptr!");
		Assert(std::char_traits<char>::length(str) > 0, "Empty strings are not allowed!");

		StringId sid = Borealis::Math::HashValue(str, std::char_traits<char>::length(str));
		std::unordered_map<StringId, const char*>::iterator it
			= g_StringIdTable.find(sid);

		if (it == g_StringIdTable.end())
		{
			// Add new string to global string table.
			g_StringIdTable[sid] = strdup(str);
		}

		return sid;
	}

	Types::StringId String(const char* str)
	{
		return InternString(str);
	}

	const char* ValueFromStringId(Types::StringId stringId)
	{
		return g_StringIdTable.find(stringId) != g_StringIdTable.end() ? g_StringIdTable[stringId] : "";
	}
}

#endif