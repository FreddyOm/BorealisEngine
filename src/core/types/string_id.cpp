#include "string_id.h"

#if defined(BOREALIS_DEBUG) || defined(BOREALIS_RELWITHDEBINFO)
#include <unordered_map>
#include "../math/crc_hash.h"
#include <cstring>

namespace Borealis::Types
{
	// @TODO: Implement custom hash table that allocates memory from pool allocator!
	std::unordered_map<StringId, const char*> g_StringIdTable;

	StringId InternString(const char* str)
	{
		__debugbreak();
		StringId sid = Math::HashValue(str);
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