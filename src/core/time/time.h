#pragma once

#include "../../config.h"
#include "../types/types.h"

#include <chrono>

namespace Borealis::Core::Time
{
    using TimePoint = std::chrono::high_resolution_clock::time_point;

    BOREALIS_API extern double DeltaTime;

    BOREALIS_API extern double GetFrameTimeMs();
    BOREALIS_API extern double GetAvgFrameTimeMs();
    BOREALIS_API extern Types::int16 GetAvgFrameRate();
    BOREALIS_API extern const TimePoint Now();
    BOREALIS_API extern const float GetDurationInMs(const TimePoint first, const TimePoint second);
}    // namespace Borealis::Core::Time