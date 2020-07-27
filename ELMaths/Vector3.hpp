#pragma once
#include "Maths.hpp"
#include <ELCore/ByteReader.hpp>
#include <ELCore/ByteWriter.hpp>
#include <ELCore/String.hpp>

template <typename T = float>
union Vector3T
{
private:
	T _data[3];

public:
	struct
	{
		T x;
		T y;
		T z;
	};

	Vector3T(const T& x = (T)0, const T& y = (T)0, const T& z = (T)0) : x(x), y(y), z(z) {}
	Vector3T(const Vector3T& other) : x(other.x), y(other.y), z(other.z) {}
	
	const T* GetData() const { return _data; }

	template <typename CAST>
	operator Vector3T<CAST>() const { return Vector3T<CAST>((CAST)x, (CAST)y, (CAST)z); }

	T& operator[](int index) { return _data[index]; }
	const T& operator[](int index) const { return _data[index]; }

	Vector3T& operator=(const Vector3T& other)
	{
		x = other.x;
		y = other.y;
		z = other.z;
		return *this;
	}

	Vector3T& operator+=(const Vector3T& other)
	{
		x += other.x;
		y += other.y;
		z += other.z;
		return *this;
	}

	Vector3T& operator-=(const Vector3T& other)
	{
		x -= other.x;
		y -= other.y;
		z -= other.z;
		return *this;
	}

	Vector3T& operator*=(const T& scalar)
	{
		x *= scalar;
		y *= scalar;
		z *= scalar;
		return *this;
	}

	Vector3T& operator*=(const Vector3T& other)
	{
		x *= other.x;
		y *= other.y;
		z *= other.z;
		return *this;
	}

	Vector3T& operator/=(const T& scalar)
	{
		x /= scalar;
		y /= scalar;
		z /= scalar;
		return *this;
	}

	Vector3T& operator/=(const Vector3T& other)
	{
		x /= other.x;
		y /= other.y;
		z /= other.z;
		return *this;
	}

	Vector3T operator+(const Vector3T& other) const	{ return Vector3T(x + other.x, y + other.y, z + other.z); }
	Vector3T operator-(const Vector3T& other) const	{ return Vector3T(x - other.x, y - other.y, z - other.z); }
	Vector3T operator-() const { return Vector3T(-x, -y, -z); }
	Vector3T operator*(const T& scalar) const		{ return Vector3T(x * scalar,  y * scalar,  z * scalar);  }
	Vector3T operator*(const Vector3T& other) const	{ return Vector3T(x * other.x, y * other.y, z * other.z); }
	Vector3T operator/(const T& scalar) const		{ return Vector3T(x / scalar,  y / scalar,  z / scalar);  }
	Vector3T operator/(const Vector3T& other) const	{ return Vector3T(x / other.x, y / other.y, z / other.z); }

	T Dot(const Vector3T& other) const { return x * other.x + y * other.y + z * other.z; }
	T LengthSquared() const { return Dot(*this); }
	T Length() const { return Maths::SquareRoot(LengthSquared()); }

	Vector3T Cross(const Vector3T& other) const
	{
		return Vector3T(
			y * other.z - z * other.y,
			z * other.x - x * other.z,
			x * other.y - y * other.x
		);
	}

	Vector3T Normalised() const { return *this / Length(); }
	Vector3T& Normalise() { return *this /= Length(); }

	Vector3T Abs() const { return Vector3T(Maths::Abs(x), Maths::Abs(y), Maths::Abs(z)); }

	Vector3T Floor() const { return Vector3T(Maths::Floor(x), Maths::Floor(y), Maths::Floor(z)); }

	//Comparison
	bool operator==(const Vector3T& other) const { return x == other.x && y == other.y && z == other.z; }
	bool operator!=(const Vector3T& other) const { return x != other.x || y != other.y || z != other.z; }

	bool AlmostEquals(const Vector3T& other, float tolerance) const { return (*this - other).LengthSquared() <= tolerance * tolerance; }

	void Read(ByteReader& reader) { x = reader.Read<T>(); y = reader.Read<T>(); z = reader.Read<T>(); }
	void Write(ByteWriter& writer) const { writer.Write<T>(x); writer.Write<T>(y); writer.Write<T>(z); }

	String ToString(const char* seperator = ", ") const { return String::Concat(x, seperator, y, seperator, z); }
	static Vector3T FromString(const String& string)
	{
		Buffer<String> tokens = string.Split(", ");
		if (tokens.GetSize() >= 3)
			return Vector3T(tokens[0].To<T>(), tokens[1].To<T>(), tokens[2].To<T>());
		
		return Vector3T();
	}

	//
	static const Vector3T& Unit() { static Vector3T unit((T)1, (T)1, (T)1); return unit; }
};

template <typename T>
Vector3T<T> operator*(const T& scalar, const Vector3T<T>& vector) { return Vector3T<T>(scalar * vector.x, scalar * vector.y, scalar * vector.z); }

template <typename T>
static Vector3T<T> operator/(const T& scalar, const Vector3T<T>& vector) { return Vector3T<T>(scalar / vector.x, scalar / vector.y, scalar / vector.z); }

typedef Vector3T<float> Vector3;

class Rotation;

namespace VectorMaths
{
	extern const Vector3 V3X;
	extern const Vector3 V3Y;
	extern const Vector3 V3Z;

	Vector3 GetPerpendicularVector(const Vector3& unit);

	Vector3 Rotate(const Vector3& vector, const Rotation& rotation);

	inline Vector3 RotateAbout(const Vector3& vector, const Vector3& pivot, const Rotation& rotation)
	{
		return Rotate(vector - pivot, rotation) + pivot;
	}
}
