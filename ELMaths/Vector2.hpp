#pragma once
#include "Maths.hpp"
#include <ELCore/ByteReader.hpp>
#include <ELCore/ByteWriter.hpp>
#include <ELCore/String.hpp>

template <typename T = float>
union Vector2T
{
private:
	T _data[3];

public:
	struct
	{
		T x;
		T y;
	};

	Vector2T(const T& x = (T)0, const T& y = (T)0) : x(x), y(y) {}
	Vector2T(const Vector2T& other) : x(other.x), y(other.y) {}

	const T* GetData() const { return _data; }

	template <typename CAST>
	operator Vector2T<CAST>() const { return Vector2T<CAST>((CAST)x, (CAST)y); }

	T& operator[](int index) { return _data[index]; }
	const T& operator[](int index) const { return _data[index]; }

	Vector2T& operator=(const Vector2T& other)
	{
		x = other.x;
		y = other.y;
		return *this;
	}

	Vector2T& operator+=(const Vector2T& other)
	{
		x += other.x;
		y += other.y;
		return *this;
	}

	Vector2T& operator-=(const Vector2T& other)
	{
		x -= other.x;
		y -= other.y;
		return *this;
	}

	Vector2T& operator*=(const T& scalar)
	{
		x *= scalar;
		y *= scalar;
		return *this;
	}

	Vector2T& operator*=(const Vector2T& other)
	{
		x *= other.x;
		y *= other.y;
		return *this;
	}

	Vector2T& operator/=(const T& scalar)
	{
		x /= scalar;
		y /= scalar;
		return *this;
	}

	Vector2T& operator/=(const Vector2T& other)
	{
		x /= other.x;
		y /= other.y;
		return *this;
	}

	Vector2T operator+(const Vector2T& other) const { return Vector2T(x + other.x, y + other.y); }
	Vector2T operator-(const Vector2T& other) const { return Vector2T(x - other.x, y - other.y); }
	Vector2T operator-() const { return Vector2T(-x, -y); }
	Vector2T operator*(const T& scalar) const { return Vector2T(x * scalar, y * scalar); }
	Vector2T operator*(const Vector2T& other) const { return Vector2T(x * other.x, y * other.y); }
	Vector2T operator/(const T& scalar) const { return Vector2T(x / scalar, y / scalar); }
	Vector2T operator/(const Vector2T& other) const { return Vector2T(x / other.x, y / other.y); }

	T Dot(const Vector2T& other) const { return x * other.x + y * other.y; }
	T LengthSquared() const { return Dot(*this); }
	T Length() const { return Maths::SquareRoot(LengthSquared()); }

	Vector2T Normalised() const { return *this / Length(); }
	Vector2T& Normalise() { return *this /= Length(); }

	Vector2T Abs() const { return Vector2T(Maths::Abs(x), Maths::Abs(y)); }
	Vector2T Floor() const { return Vector2T(Maths::Floor(x), Maths::Floor(y)); }

	//Comparison
	bool operator==(const Vector2T& other) const { return x == other.x && y == other.y; }
	bool operator!=(const Vector2T& other) const { return x != other.x || y != other.y; }

	bool AlmostEquals(const Vector2T& other, float tolerance) const { return (*this - other).LengthSquared() <= tolerance * tolerance; }

	void Read(ByteReader& reader) { x = reader.Read<T>(); y = reader.Read<T>(); }
	void Write(ByteWriter& writer) const { writer.Write<T>(x); writer.Write<T>(y); }

	String ToString(const char* seperator = ", ") const { return String::Concat(x, seperator, y); }
	static Vector2T FromString(const String& string)
	{
		Buffer<String> tokens = string.Split(", ");
		if (tokens.GetSize() >= 2)
			return Vector2T(tokens[0].To<T>(), tokens[1].To<T>());

		return Vector2T();
	}
};

template <typename T>
Vector2T<T> operator*(const T& scalar, const Vector2T<T>& vector) { return Vector2T<T>(scalar * vector.x, scalar * vector.y); }

template <typename T>
static Vector2T<T> operator/(const T& scalar, const Vector2T<T>& vector) { return Vector2T<T>(scalar / vector.x, scalar / vector.y); }

typedef Vector2T<float> Vector2;
