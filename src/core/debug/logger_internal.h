#pragma once

#include <stdio.h>
#include "../types/types.h"

namespace Borealis::Debug
{
	struct DebugInfoDesc;

	int LogMessageInternal(DebugInfoDesc debugInfoDesc);
}
