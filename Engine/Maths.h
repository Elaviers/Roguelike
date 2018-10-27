#pragma once
namespace Maths
{
	constexpr double PI = 3.14159265358979323846;
	const float DEGS_PER_RAD_F = 180.f / (float)PI;
	const float RADS_PER_DEG_F = (float)PI / 180.f;

	template <typename T>
	inline T Abs(const T& value) { return value < 0 ? -value : value; }

	float SquareRoot(float x);

	float Sine(float radians);
	float Cosine(float radians);
	float Tangent(float radians);
	float ArcSine(float x);
	float ArcCosine(float x);
	float ArcTangent(float x);

	inline float SineDegrees(float degrees) { return Sine(RADS_PER_DEG_F * degrees); };
	inline float CosineDegrees(float degrees) { return Cosine(RADS_PER_DEG_F * degrees); };
	inline float TangentDegrees(float degrees) { return Tangent(RADS_PER_DEG_F * degrees); };

	inline float ArcSineDegrees(float x) { return DEGS_PER_RAD_F * ArcSine(x); }
	inline float ArcCoineDegrees(float x) { return DEGS_PER_RAD_F * ArcCosine(x); }
	inline float ArcTangentDegrees(float x) { return DEGS_PER_RAD_F * ArcTangent(x); }
	inline float ArcTangentDegrees2(float x, float y)
	{ 
		if (x) return ArcTangentDegrees(y / x);
		if (y < 0.f) return -90.f;
		return 90.f;
	}

	template <typename T>
	inline T Clamp(T min, T max, T x)
	{
		if (x < min)
			return min;

		if (x > max)
			return max;

		return x;
	}
}
