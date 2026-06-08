#include "math.h"

namespace Borealis::Math
{
	BOREALIS_API float Clamp(float value, float min, float max)
	{
		return value >= min && value <= max ? value : value < min ? min : max;
	}

	BOREALIS_API float Clamp01(float value)
	{
		return Clamp(value, 0, 1);
	}

}