#pragma once
#include "Maths.h"

template <typename T, int SIZE>
class Vector
{
private:
	T _data[SIZE];

public:
	Vector() : _data() {}
	Vector(float x, float y)					{ static_assert(SIZE == 2, "Vector size differs from argument count"); _data[0] = x; _data[1] = y; }
	Vector(float x, float y, float z)			{ static_assert(SIZE == 3, "Vector size differs from argument count"); _data[0] = x; _data[1] = y; _data[2] = z; }
	Vector(float x, float y, float z, float w)	{ static_assert(SIZE == 4, "Vector size differs from argument count"); _data[0] = x; _data[1] = y; _data[2] = z; _data[3] = w; }

	~Vector() {}

	T LengthSquared() const
	{
		T lengthSq = 0;
		for (int i = 0; i < SIZE; ++i)
			lengthSq += _data[i] * _data[i];

		return lengthSq;
	}

	inline float Length() const { return Maths::SquareRoot(LengthSquared()); }
	inline void Normalise() { *this /= Length(); }

	inline T&		operator[](int index)		{ return _data[index]; }
	inline const T& operator[](int index) const { return _data[index]; }

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

	Vector& operator+=(T other)
	{
		for (int i = 0; i < SIZE; ++i)
			_data[i] += other;

		return *this;
	}

	Vector& operator-=(T other)
	{
		for (int i = 0; i < SIZE; ++i)
			_data[i] -= other;

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

	Vector operator*(T other) const				{ Vector v(*this); v *= other; return v; }
	Vector operator/(T other) const				{ Vector v(*this); v /= other; return v; }
	Vector operator+(T other) const				{ Vector v(*this); v += other; return v; }
	Vector operator-(T other) const				{ Vector v(*this); v -= other; return v; }
	Vector operator*(const Vector& other) const { Vector v(*this); v *= other; return v; }
	Vector operator/(const Vector& other) const { Vector v(*this); v /= other; return v; }
	Vector operator+(const Vector& other) const { Vector v(*this); v += other; return v; }
	Vector operator-(const Vector& other) const { Vector v(*this); v -= other; return v; }

	bool operator==(const Vector &other) const 
	{
		for (int i = 0; i < SIZE; ++i) 
			if (_data[i] != other._data[i]) 
				return false; 
		
		return true; }
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
typedef Vector<float, 4> Vector4;
