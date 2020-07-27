#pragma once
#include <ELCore/Types.hpp>
#include <ELSys/Debug.hpp>
#include <math.h>

class Quaternion;

namespace Maths
{
	constexpr double PI = 3.14159265358979323846;
	constexpr float PI_F = (float)PI;
	constexpr const double DEGS_PER_RAD = 180.0 / PI;
	constexpr const double RADS_PER_DEG = PI / 180.0;

	template <typename T>
	T Abs(const T& value) { return value < 0 ? -value : value; }

	template <typename T>
	T Lerp(const T& from, const T& to, float alpha) { return (T)((from * (1.f - alpha)) + (to * alpha)); }

	template <typename T>
	inline const T& Min(const T* elements, size_t size)
	{
		Debug::Assert(size > 0, "Maths::Min called with 0 size");

		const T* min = &elements[0];
		for (size_t i = 1; i < size; ++i)
			if (elements[i] < *min)
				min = &elements[i];

		return *min;
	}

	template <typename T>
	inline const T& Max(const T* elements, size_t size)
	{
		Debug::Assert(size > 0, "Maths::Max called with 0 size");

		const T* max = &elements[0];
		for (size_t i = 1; i < size; ++i)
			if (elements[i] > *max)
				max = &elements[i];

		return *max;
	}

	template <typename T>
	inline T Min(const T& a, const T& b) { return a < b ? a : b; }

	template <typename T>
	inline T Max(const T& a, const T& b) { return a > b ? a : b; }

	template <typename T>
	inline T Clamp(const T& x, const T& lower, const T& upper)
	{
		if (x < lower)
			return lower;
		if (x > upper)
			return upper;

		return x;
	}

	template<>
	Quaternion Lerp(const Quaternion& from, const Quaternion& to, float alpha);

	inline float Round(float x) 
	{
		int32 asInt = (int32)x;
		float dec = x - asInt;

		return dec >= .5f ? ((float)asInt + 1.f) : (float)asInt;
	}

	inline float Round(float x, float nearest)
	{
		float inv = 1.f / nearest;
		return Round(x * inv) / inv;
	}

	inline float Floor(float x)
	{
		return floorf(x);
	}

	inline float Floor(float x, float nearest)
	{
		float inv = 1.f / nearest;
		return Floor(x * inv) / inv;
	}

	inline int Trunc(float x)
	{
		return (int)x;
	}

	inline float Trunc(float x, float nearest)
	{
		float inv = 1.f / nearest;
		return Trunc(x * inv) / inv;
	}

	float SquareRoot(float x);

	template<typename T>
	inline T Pow(T base, unsigned int index)
	{
		T result = (T)1;
		while (index--) result *= base;
		return result;
	}


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

	float ArcTangentDegrees2(float y, float x);

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
	
	inline float SmallestAngularDiff(float to, float from)
	{
		if (to == from) return 0.f;

		float diff = to - from;
		while (diff > 180.f)
			diff -= 360.f;
		while (diff < -180.f)
			diff += 360.f;

		return diff;
	}

	template<typename T>
	inline bool AlmostEquals(const T& a, const T& b, const T& tolerance)
	{
		return (a >= b - tolerance && a <= b + tolerance);
	}
}
