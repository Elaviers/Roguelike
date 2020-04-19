#pragma once
#include "Vector3.hpp"
#include <xmmintrin.h>

union Vector4
{
	_declspec(align(16)) float _data[4];

public:
	struct
	{
		float x;
		float y;
		float z;
		float w;
	};

	Vector4(float x, float y, float z, float w = 1.f) : _data {x, y, z, w} {}
	Vector4(float a = 0.f) : Vector4(a, a, a) {}
	Vector4(const Vector3 &v3, float w = 1.f) : Vector4(v3.x, v3.y, v3.z, w) {}

	Vector4(__m128 simd) { _mm_store_ps(_data, simd); }

	const float* GetData() const { return _data; }
	
	Vector4& operator=(__m128 simd)
	{
		_mm_store_ps(_data, simd);
		return *this;
	}

	Vector4& operator=(const Vector4 &other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		w = other.w;
		return *this;
	}

	__m128 LoadSIMD() const { return _mm_load_ps(_data); }

	float& operator[](int element) { return _data[element]; }
	const float& operator[](int element) const { return _data[element]; }

	Vector4& operator+=(const Vector4 &other) { return *this = _mm_add_ps(LoadSIMD(), other.LoadSIMD()); }
	Vector4& operator-=(const Vector4 &other) { return *this = _mm_sub_ps(LoadSIMD(), other.LoadSIMD()); }
	Vector4& operator*=(const Vector4 &other) { return *this = _mm_mul_ps(LoadSIMD(), other.LoadSIMD()); }
	Vector4& operator/=(const Vector4 &other) { return *this = _mm_div_ps(LoadSIMD(), other.LoadSIMD()); }
	Vector4 operator+(const Vector4 &other) const { return Vector4(_mm_add_ps(LoadSIMD(), other.LoadSIMD())); }
	Vector4 operator-(const Vector4 &other) const { return Vector4(_mm_sub_ps(LoadSIMD(), other.LoadSIMD())); }
	Vector4 operator*(const Vector4 &other) const { return Vector4(_mm_mul_ps(LoadSIMD(), other.LoadSIMD())); }
	Vector4 operator/(const Vector4 &other) const { return Vector4(_mm_div_ps(LoadSIMD(), other.LoadSIMD())); }

	Vector4& operator*=(float f) { x *= f; y *= f; z *= f; w *= f; return *this; }
	Vector4& operator/=(float f) { x /= f; y /= f; z /= f; w /= f; return *this; }
	Vector4 operator*(float f) { Vector4 v(*this); return v *= f; }
	Vector4 operator/(float f) { Vector4 v(*this); return v /= f; }

	bool operator!=(const Vector4& other) const
	{
		return x != other.x || y != other.y || z != other.z || w != other.w;
	}

	bool operator==(const Vector4& other) const 
	{ 
		return x == other.x && y == other.y && z == other.z && w == other.w;
	}

	bool AlmostEquals(const Vector4& other, float tolerance) const
	{
		return ((*this - other).LengthSquared() <= tolerance * tolerance);
	}

	float LengthSquared() const
	{
		return x * x + y * y + z * z + w * w;
	}

	float Length() const
	{
		return Maths::SquareRoot(LengthSquared());
	}

	void Normalise()
	{
		*this /= Length();
	}
};

inline Vector4 operator-(float f, const Vector4& v)
{
	return Vector4(f - v.x, f - v.y, f - v.z, f - v.w);
}
