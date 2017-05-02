#pragma once

#include <math.h>

class TMathUtil
{
public:
	static float GetRadByDegree(float degree) 
	{
		return (degree*PI / 180.0f);
	}

	static float InvSqrt(float F)
	{
		return 1.0f / sqrtf(F);
	}


public:
	static float PI;
};