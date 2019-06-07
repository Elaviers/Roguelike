#pragma once
#include "Matrix.hpp"
#include "Vector.hpp"

/*
	Quaternion
	Unit Quaternion

	Euler conversions are in YXZ order, all angles are clockwise
*/

class Quaternion
{
	Vector4 _data;

	float& _x = _data[0];
	float& _y = _data[1];
	float& _z = _data[2];
	float& _w = _data[3];

	inline static Quaternion _FromAxisW(const Vector3 &axis, float w) { return Quaternion(Vector4(axis[0], axis[1], axis[2], w)); }

public:
	Quaternion() : _data(0.f, 0.f, 0.f, 1.f) {}

	//X, Y, Z, W
	Quaternion(const Vector4& data) : _data(data) { _data.Normalise(); }

	Quaternion(const Vector3& axis, float angle);

	Quaternion(const Vector3& euler);

	Quaternion(const Quaternion& other) : _data(other._data) {}

	~Quaternion() {}

	Vector3 ToEuler() const;

	Mat4 ToMatrix() const;

	inline Quaternion Inverse() const
	{ 
		return Quaternion(Vector4(-1.f * _x, -1.f * _y, -1.f * _z, _w));
	}

	inline Quaternion& operator=(const Quaternion& other)
	{
		_data = other._data;

		return *this;
	}

	Quaternion operator*(const Quaternion& other) const;

	inline Vector3 Transform(const Vector3& point) const
	{
		Quaternion result = Inverse() * Quaternion(Vector4(point[0], point[1], point[2], 0.f)) * *this;

		return Vector3(result._x, result._y, result._z);
	}

	inline Vector3 GetForwardVector() const
	{
		return Transform(Vector3(0.f, 0.f, 1.f));
	}

	inline Vector3 GetRightVector() const
	{
		return Transform(Vector3(1.f, 0.f, 0.f));
	}

	inline Vector3 GetUpVector() const
	{
		return Transform(Vector3(0.f, 1.f, 0.f));
	}

	static Quaternion Lerp(const Quaternion& from, const Quaternion& to, float alpha);
};