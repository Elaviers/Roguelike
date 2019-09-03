#include "Maths.hpp"
#include "Quaternion.hpp"
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

	template <>
	Quaternion Lerp(const Quaternion& from, const Quaternion& to, float alpha) { return Quaternion::Lerp(from, to, alpha); }

	double SineD(double radians)
	{
		return sin(radians);
	}

	double CosineD(double radians)
	{
		return cos(radians);
	}

	double TangentD(double radians)
	{
		return tan(radians);
	}

	double ArcSineD(double x)
	{
		return asin(x);
	}

	double ArcCosineD(double x)
	{
		return asin(x);
	}

	double ArcTangentD(double x)
	{
		return atan(x);
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

	void SetRandomSeed(unsigned int seed)
	{
		srand(seed);
	}

	float ArcTangentDegrees2(float y, float x)
	{
		if (x > 0.f)
			return ArcTangentDegrees(y / x);
		else if (x < 0.f)
			return ArcTangentDegrees(y / x) + ((y < 0.f) ? -180.f : 180.f);

		//x == 0
		if (y == 0.f)
			return 0.f;

		return y < 0.f ? -90.f : 90.f;
	}
}
