#include "Maths.h"
#include <math.h>

namespace Maths
{
	float Round(float x)
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
		return Round(sinf(radians));
	}

	float Cosine(float radians)
	{
		return Round(cosf(radians));
	}

	float Tangent(float radians)
	{
		return Round(tanf(radians));
	}
}
