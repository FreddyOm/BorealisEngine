#pragma once

#include "../../config.h"
#include "../types/types.h"

namespace Borealis::Math::Random
{
    BOREALIS_API Types::uint64 Next32();
    BOREALIS_API Types::uint64 Next64();
}