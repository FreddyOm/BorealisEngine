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
		Assert(strlen(str) > 0, "Empty strings are not allowed!");

		StringId sid = Math::HashValue(str, strlen(str));
		std::unordered_map<StringId, const char*>::iterator it
			= g_StringIdTable.find(sid);

		if (it == g_StringIdTable.end())
		{
			// Add new string to global string table.
			g_StringIdTable[sid] = strdup(str);
		}

		return sid;
	}
}

#endif