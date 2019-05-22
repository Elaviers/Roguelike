#pragma once
#include "Types.h"

namespace Maths
{
	constexpr double PI = 3.14159265358979323846;
	constexpr const double DEGS_PER_RAD = 180.0 / PI;
	constexpr const double RADS_PER_DEG = PI / 180.0;

	template <typename T>
	inline T Abs(const T& value) { return value < 0 ? -value : value; }

	template <typename T>
	inline T Lerp(const T& from, const T& to, float alpha) { return (T)((from * (1.f - alpha)) + (to * alpha)); }

	inline float Round(float x) 
	{
		uint32 asInt = (uint32)x;
		float dec = x - asInt;

		return (dec >= .5f) ? (float)asInt : ((float)asInt + 1.f);
	}

	float SquareRoot(float x);

	float Sine(float radians);
	float Cosine(float radians);
	float Tangent(float radians);
	float ArcSine(float x);
	float ArcCosine(float x);
	float ArcTangent(float x);

	inline float SineDegrees(float degrees) { return Sine((float)RADS_PER_DEG * degrees); };
	inline float CosineDegrees(float degrees) { return Cosine((float)RADS_PER_DEG * degrees); };
	inline float TangentDegrees(float degrees) { return Tangent((float)RADS_PER_DEG * degrees); };
	inline float ArcSineDegrees(float x) { return (float)DEGS_PER_RAD * ArcSine(x); }
	inline float ArcCoineDegrees(float x) { return (float)DEGS_PER_RAD * ArcCosine(x); }
	inline float ArcTangentDegrees(float x) { return (float)DEGS_PER_RAD * ArcTangent(x); }

	inline float ArcTangentDegrees2(float y, float x)
	{ 
		if (x) return ArcTangentDegrees(y / x);
		if (y < 0.f) return -90.f;
		return 90.f;
	}

	double SineD(double radians);
	double CosineD(double radians);
	double TangentD(double radians);
	double ArcSineD(double x);
	double ArcCosineD(double x);
	double ArcTangentD(double x);

	inline double SineDegreesD(double degrees) { return SineD(RADS_PER_DEG * degrees); };
	inline double CosineDegreesD(double degrees) { return CosineD(RADS_PER_DEG * degrees); };
	inline double TangentDegreesD(double degrees) { return TangentD(RADS_PER_DEG * degrees); };
	inline double ArcSineDegreesD(double x) { return DEGS_PER_RAD * ArcSineD(x); }
	inline double ArcCoineDegreesD(double x) { return DEGS_PER_RAD * ArcCosineD(x); }
	inline double ArcTangentDegreesD(double x) { return DEGS_PER_RAD * ArcTangentD(x); }

	inline double ArcTangentDegrees2D(double y, double x)
	{
		if (x) return ArcTangentDegreesD(y / x);
		if (y < 0.f) return -90.0;
		return 90.0;
	}

	float Random();
}
