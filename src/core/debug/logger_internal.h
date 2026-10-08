#pragma once

#include "../types/types.h"

#include <stdio.h>

namespace Borealis::Debug
{
    struct DebugInfoDesc;

    Borealis::Types::int16 LogMessageInternal(DebugInfoDesc debugInfoDesc);
}    // namespace Borealis::Debug