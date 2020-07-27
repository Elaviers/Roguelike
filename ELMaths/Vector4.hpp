#pragma once
#include "Vector3.hpp"
#include <ELCore/ByteReader.hpp>
#include <ELCore/ByteWriter.hpp>
#include <ELCore/String.hpp>
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

	Vector3 GetXYZ() const { return Vector3(x, y, z); }
	
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

	void Read(ByteReader& reader) { x = reader.Read_float(); y = reader.Read_float(); z = reader.Read_float(); w = reader.Read_float(); }
	void Write(ByteWriter& writer) const { writer.Write_float(x); writer.Write_float(y); writer.Write_float(z); writer.Write_float(w); }

	String ToString(const char* seperator = ", ") const { return String::Concat(x, seperator, y, seperator, z, seperator, w); }
	static Vector4 FromString(const String& string)
	{
		Buffer<String> tokens = string.Split(", ");
		if (tokens.GetSize() >= 4)
			return Vector4(tokens[0].ToFloat(), tokens[1].ToFloat(), tokens[2].ToFloat());

		return Vector4();
	}
};

inline Vector4 operator-(float f, const Vector4& v)
{
	return Vector4(f - v.x, f - v.y, f - v.z, f - v.w);
}
