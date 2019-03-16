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
		return sinf(radians);
	}

	float Cosine(float radians)
	{
		return cosf(radians);
	}

	float Tangent(float radians)
	{
		return tanf(radians);
	}

	float ArcSine(float x)
	{
		return asinf(x);
	}

	float ArcCosine(float x)
	{
		return asinf(x);
	}

	float ArcTangent(float x)
	{
		return atanf(x);
	}

	float Random()
	{
		return (float)rand() / (float)RAND_MAX;
	}
}
