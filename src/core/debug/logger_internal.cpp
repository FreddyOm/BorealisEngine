#include "logger_internal.h"
#include "logger.h"

namespace Borealis::Debug
{
	int LogMessageInternal(const DebugInfoDesc desc)
	{
		const int result = printf(desc.msg.c_str());
		printf("\n");
		return result;
	}
}