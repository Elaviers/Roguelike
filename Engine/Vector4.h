#pragma once
#include <xmmintrin.h>

class Vector4
{
	_declspec(align(16)) float _data[4];

public:
	float &x = _data[0];
	float &y = _data[1];
	float &z = _data[2];

	Vector4(float x, float y, float z, float w = 1.f) : _data {x, y, z, w} {}
	Vector4(float x = 0.f) : Vector4(x, x, x) {}

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

	inline Vector4& operator*=(float f) { _data[0] *= f; _data[1] *= f; _data[2] *= f; return *this; }
	inline Vector4& operator/=(float f) { _data[0] /= f; _data[1] /= f; _data[2] /= f; return *this; }
	inline Vector4 operator*(float f) { Vector4 v(*this); return v *= f; }
	inline Vector4 operator/(float f) { Vector4 v(*this); return v /= f; }
};
