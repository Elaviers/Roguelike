#pragma once
#include "Maths.h"

template <typename T, int SIZE>
class Vector
{
protected:
	T _data[SIZE];

public:
	Vector() : _data() {}
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

	inline float Length() const { return Maths::SquareRoot(LengthSquared()); }
	inline Vector Normal() { return *this / Length(); }
	inline Vector& Normalise() { *this /= Length(); return *this; }

	inline T&		operator[](int index) { return _data[index]; }
	inline const T& operator[](int index) const { return _data[index]; }

	template <typename CAST>
	operator Vector<CAST, SIZE>()
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

	bool operator==(const Vector &other) const
	{
		for (int i = 0; i < SIZE; ++i)
			if (_data[i] != other._data[i])
				return false;

		return true;
	}

	static T Dot(const Vector &a, const Vector &b)
	{
		float result = 0.f;
		for (int i = 0; i < SIZE; ++i)
			result += a[i] * b[i];

		return result;
	}

	inline Vector Abs() const
	{
		Vector v;
		for (int i = 0; i < SIZE; ++i)
			v[i] = Maths::Abs(_data[i]);

		return v;
	}
};

template<typename T, int SIZE>
inline Vector<T, SIZE> operator*(T x, const Vector<T, SIZE> &vec)
{
	return vec * x;
}

template<typename T, int SIZE>
Vector<T, SIZE> operator/(T x, const Vector<T, SIZE> &vec)
{
	Vector<T, SIZE> v(vec);
	for (int i = 0; i < SIZE; ++i)
		v[i] = x / vec[i];

	return v;
}

typedef Vector<float, 2> Vector2;
typedef Vector<float, 3> Vector3;

namespace VectorMaths
{
	Vector3 GetForwardVector(const Vector3 &rotation);
	Vector3 GetRightVector(const Vector3 &rotation);
	Vector3 GetUpVector(const Vector3 &rotation);

	Vector3 Rotate(const Vector3 &vector, const Vector3 &rotation);
}
