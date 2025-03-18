#include "logger_internal.h"
#include "logger.h"

namespace Borealis::Debug
{
	Borealis::Types::int16 LogMessageInternal(const DebugInfoDesc desc)
	{
		const Borealis::Types::int16 result = printf(desc.msg.c_str());
		printf("\n");
		return result;
	}
}