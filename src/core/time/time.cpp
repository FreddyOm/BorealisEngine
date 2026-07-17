#include "time.h"
#include "time_internal.h"
#include "../types/types.h"

namespace Borealis::Core::Time
{
	double DeltaTime = 0;
	double FrameTimeMs = 0;

	TimePoint frameStartTime;
	TimePoint frameEndTime;

#define AVG_BUF_LEN 256
	alignas(64) double avgFrameTimeBuf[AVG_BUF_LEN] = { 0 };
	alignas(64) Types::int16 avgFrameRateBuf[AVG_BUF_LEN] = {};
	Types::int16 avgBufIndex = 0;
	bool avgBufFilled = false;


	BOREALIS_API double GetFrameTimeMs()
	{
		return FrameTimeMs;
	}

	BOREALIS_API double GetAvgFrameTimeMs()
	{
		double sum = 0;
		Types::int16 count = avgBufFilled ? AVG_BUF_LEN : avgBufIndex;
		for (Types::int16 i = 0; i < count; ++i)
		{
			sum += avgFrameTimeBuf[i];
		}
		return count > 0 ? sum / count : 0;
	}

	BOREALIS_API Types::int16 GetAvgFrameRate()
	{
		Types::int32 sum = 0;
		Types::int16 count = avgBufFilled ? AVG_BUF_LEN : avgBufIndex;
		for (Types::int16 i = 0; i < count; ++i)
		{
			sum += avgFrameRateBuf[i];
		}
		return count > 0 ? sum / count : 0;
	}

	BOREALIS_API const float GetDurationInMs(const TimePoint first, const TimePoint second)
	{
		auto duration = std::chrono::duration_cast<std::chrono::microseconds>(second - first);
		return duration.count() / 1000.0;
	}

	BOREALIS_API const TimePoint Now()
	{
		return std::chrono::high_resolution_clock::now();
	}



	void StartFrameTimer()
	{
		frameStartTime = std::chrono::high_resolution_clock::now();
	}

	void EndFrameTimer()
	{
		frameEndTime = std::chrono::high_resolution_clock::now();
		auto frameTime = std::chrono::duration_cast<std::chrono::microseconds>(frameEndTime - frameStartTime);
		DeltaTime = frameTime.count() / 1000000.0;
		FrameTimeMs = frameTime.count() / 1000.0;
		avgFrameTimeBuf[avgBufIndex] = FrameTimeMs;
		avgFrameRateBuf[avgBufIndex] = (Types::int16) (1.0 / DeltaTime);
		avgBufIndex = (avgBufIndex + 1) % AVG_BUF_LEN;
		if (avgBufIndex == 0)
		{
			avgBufFilled = true;
		}
	}

}
