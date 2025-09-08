#include "FloatHelper.h"

float FloatHelper::RpmToInterval(const float Rpm)
{
	if (Rpm <= 0.0f)
	{
		return FLT_MAX;
	}

	return 60.0f / Rpm;
}
