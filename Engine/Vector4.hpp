#pragma once
#include "Vector.hpp"
#include <xmmintrin.h>

class Vector4
{
	_declspec(align(16)) float _data[4];

public:
	Vector4(float x, float y, float z, float w = 1.f) : _data {x, y, z, w} {}
	Vector4(float a = 0.f) : Vector4(a, a, a) {}
	Vector4(const Vector3 &v3) : Vector4(v3[0], v3[1], v3[2], 1.f) {}

	Vector4(__m128 simd) { _mm_store_ps(_data, simd); }
	
	inline Vector4& operator=(__m128 simd)
	{
		_mm_store_ps(_data, simd);
		return *this;
	}

	inline Vector4& operator=(const Vector4 &other)
	{
		_data[0] = other._data[0];
		_data[1] = other._data[1];
		_data[2] = other._data[2];
		_data[3] = other._data[3];
		return *this;
	}

	inline __m128 LoadSIMD() const { return _mm_load_ps(_data); }

	inline float& operator[](int element) { return _data[element]; }
	inline const float& operator[](int element) const { return _data[element]; }

	inline Vector4& operator+=(const Vector4 &other) { return *this = _mm_add_ps(LoadSIMD(), other.LoadSIMD()); }
	inline Vector4& operator-=(const Vector4 &other) { return *this = _mm_sub_ps(LoadSIMD(), other.LoadSIMD()); }
	inline Vector4& operator*=(const Vector4 &other) { return *this = _mm_mul_ps(LoadSIMD(), other.LoadSIMD()); }
	inline Vector4& operator/=(const Vector4 &other) { return *this = _mm_div_ps(LoadSIMD(), other.LoadSIMD()); }
	inline Vector4 operator+(const Vector4 &other) const { return Vector4(_mm_add_ps(LoadSIMD(), other.LoadSIMD())); }
	inline Vector4 operator-(const Vector4 &other) const { return Vector4(_mm_sub_ps(LoadSIMD(), other.LoadSIMD())); }
	inline Vector4 operator*(const Vector4 &other) const { return Vector4(_mm_mul_ps(LoadSIMD(), other.LoadSIMD())); }
	inline Vector4 operator/(const Vector4 &other) const { return Vector4(_mm_div_ps(LoadSIMD(), other.LoadSIMD())); }

	inline Vector4& operator*=(float f) { _data[0] *= f; _data[1] *= f; _data[2] *= f; _data[3] *= f; return *this; }
	inline Vector4& operator/=(float f) { _data[0] /= f; _data[1] /= f; _data[2] /= f; _data[3] /= f; return *this; }
	inline Vector4 operator*(float f) { Vector4 v(*this); return v *= f; }
	inline Vector4 operator/(float f) { Vector4 v(*this); return v /= f; }

	inline float LengthSquared() const
	{
		return _data[0] * _data[0] + _data[1] * _data[1] + _data[2] * _data[2] + _data[3] * _data[3];
	}

	inline float Length() const
	{
		return Maths::SquareRoot(LengthSquared());
	}

	inline void Normalise()
	{
		*this /= Length();
	}
};
