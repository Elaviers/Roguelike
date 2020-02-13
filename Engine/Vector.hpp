#pragma once
#include "Maths.hpp"

template <typename T, int SIZE>
class Vector
{
protected:
	T _data[SIZE];

public:
	Vector() : _data() {}
	Vector(const Vector& other) : _data()
	{
		for (int i = 0; i < SIZE; ++i)
			_data[i] = other._data[i];
	}

	Vector(T x, T y) { static_assert(SIZE == 2, "Vector size differs from argument count"); _data[0] = x; _data[1] = y; }
	Vector(T x, T y, T z) { static_assert(SIZE == 3, "Vector size differs from argument count"); _data[0] = x; _data[1] = y; _data[2] = z; }
	Vector(T x, T y, T z, T w) { static_assert(SIZE == 4, "Vector size differs from argument count"); _data[0] = x; _data[1] = y; _data[2] = z; _data[3] = w; }

	T LengthSquared() const
	{
		T lengthSq = 0;
		for (int i = 0; i < SIZE; ++i)
			lengthSq += _data[i] * _data[i];

		return lengthSq;
	}

	float Length() const { return Maths::SquareRoot(LengthSquared()); }
	Vector Normalised() const { return *this / Length(); }
	Vector& Normalise() { *this /= Length(); return *this; }

	T&		operator[](int index) { return _data[index]; }
	const T& operator[](int index) const { return _data[index]; }

	template <typename CAST>
	operator Vector<CAST, SIZE>() const
	{
		Vector<CAST, SIZE> result;
		for (int i = 0; i < SIZE; ++i)
			result[i] = (CAST)_data[i];

		return result;
	}

	Vector& operator=(const Vector& other)
	{
		for (int i = 0; i < SIZE; ++i)
			_data[i] = other._data[i];

		return *this;
	}

	Vector& operator*=(T other)
	{
		for (int i = 0; i < SIZE; ++i)
			_data[i] *= other;

		return *this;
	}

	Vector& operator/=(T other)
	{
		for (int i = 0; i < SIZE; ++i)
			_data[i] /= other;

		return *this;
	}

	Vector& operator*=(const Vector& other)
	{
		for (int i = 0; i < SIZE; ++i)
			_data[i] *= other._data[i];

		return *this;
	}

	Vector& operator/=(const Vector& other)
	{
		for (int i = 0; i < SIZE; ++i)
			_data[i] /= other._data[i];

		return *this;
	}

	Vector& operator+=(const Vector& other)
	{
		for (int i = 0; i < SIZE; ++i)
			_data[i] += other._data[i];

		return *this;
	}

	Vector& operator-=(const Vector& other)
	{
		for (int i = 0; i < SIZE; ++i)
			_data[i] -= other._data[i];

		return *this;
	}

	Vector operator*(T other) const { Vector v(*this); v *= other; return v; }
	Vector operator/(T other) const { Vector v(*this); v /= other; return v; }
	Vector operator*(const Vector& other) const { Vector v(*this); v *= other; return v; }
	Vector operator/(const Vector& other) const { Vector v(*this); v /= other; return v; }
	Vector operator+(const Vector& other) const { Vector v(*this); v += other; return v; }
	Vector operator-(const Vector& other) const { Vector v(*this); v -= other; return v; }

	Vector operator-() const { return -1.f * *this; }

	bool operator!=(const Vector& other) const
	{
		for (int i = 0; i < SIZE; ++i)
			if (_data[i] != other._data[i])
				return true;

		return false;
	}

	bool operator==(const Vector& other) const { return !operator!=(other); }

	bool AlmostEqual(const Vector& other, float tolerance) const
	{
		return ((*this - other).LengthSquared() <= tolerance * tolerance);
	}

	static Vector Cross(const Vector& a, const Vector& b)
	{
		static_assert(SIZE == 3, "Cross product is only implemented for 3D vectors right now...");

		return Vector(a[1] * b[2] - a[2] * b[1], a[2] * b[0] - a[0] * b[2], a[0] * b[1] - a[1] * b[0]);
	}

	static T Dot(const Vector &a, const Vector &b)
	{
		T result = 0.f;
		for (int i = 0; i < SIZE; ++i)
			result += a[i] * b[i];

		return result;
	}

	Vector Abs() const
	{
		Vector v;
		for (int i = 0; i < SIZE; ++i)
			v[i] = Maths::Abs(_data[i]);

		return v;
	}
};

template<typename T, int SIZE>
Vector<T, SIZE> operator*(T x, const Vector<T, SIZE> &vec)
{
	return vec * x;
}

template<typename T, int SIZE>
Vector<T, SIZE> operator/(T x, const Vector<T, SIZE> &vec)
{
	Vector<T, SIZE> v;
	for (int i = 0; i < SIZE; ++i)
		v[i] = x / vec[i];

	return v;
}

template<typename T, int SIZE>
Vector<T, SIZE> operator-(T x, const Vector<T, SIZE>& vec)
{
	Vector<T, SIZE> v;
	for (int i = 0; i < SIZE; ++i)
		v[i] = x - vec[i];

	return v;
}

typedef Vector<float, 2> Vector2;
typedef Vector<float, 3> Vector3;

class Rotation;

namespace VectorMaths
{
	extern const Vector3 V3X;
	extern const Vector3 V3Y;
	extern const Vector3 V3Z;

	Vector3 GetPerpendicularVector(const Vector3& unit);

	Vector3 Rotate(const Vector3 &vector, const Rotation &rotation);

	inline Vector3 RotateAbout(const Vector3& vector, const Vector3& pivot, const Rotation& rotation)
	{
		return Rotate(vector - pivot, rotation) + pivot;
	}
}
