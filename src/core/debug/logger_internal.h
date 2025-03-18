#pragma once

#include <stdio.h>
#include "../types/types.h"

namespace Borealis::Debug
{
	struct DebugInfoDesc;

	Borealis::Types::int16 LogMessageInternal(DebugInfoDesc debugInfoDesc);
}
