#include "Maths.h"
#include <stdlib.h>
#include <math.h>

namespace Maths
{
	float RoundOff(float x)
	{
		if (x <= 0.0001f && x >= -0.0001f)
			x = 0.f;

		return x;
	}

	float SquareRoot(float x)
	{
		return sqrtf(x);
	}

	float Sine(float radians)
	{
		return RoundOff(sinf(radians));
	}

	float Cosine(float radians)
	{
		return RoundOff(cosf(radians));
	}

	float Tangent(float radians)
	{
		return RoundOff(tanf(radians));
	}

	float ArcSine(float x)
	{
		return RoundOff(asinf(x));
	}

	float ArcCosine(float x)
	{
		return RoundOff(asinf(x));
	}

	float ArcTangent(float x)
	{
		return RoundOff(atanf(x));
	}

	float Random()
	{
		return (float)rand() / (float)RAND_MAX;
	}
}
