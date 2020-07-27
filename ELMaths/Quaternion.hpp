#pragma once
#include "Vector4.hpp"

/*
	Quaternion
	Unit Quaternion

	Euler conversions are in YXZ order, all angles are clockwise
*/

class Quaternion
{
	union
	{
		Vector4 _data;
		struct
		{
			float _x;
			float _y;
			float _z;
			float _w;
		};
	};

	static Quaternion _FromAxisW(const Vector3 &axis, float w) { return Quaternion(Vector4(axis.x, axis.y, axis.z, w)); }

public:
	Quaternion() : _data(0.f, 0.f, 0.f, 1.f) {}

	//X, Y, Z, W
	Quaternion(const Vector3& axis, float angle);
	Quaternion(const Vector4& data) : _data(data) { _data.Normalise(); }
	Quaternion(const Vector3& euler) { *this = FromEulerYXZ(euler); }
	Quaternion(const Quaternion& other) : _data(other._data) {}

	~Quaternion() {}

	static Quaternion FromEulerYXZ(const Vector3& euler);
	static Quaternion FromEulerZYX(const Vector3& euler);

	const Vector4& GetData() const { return _data; }

	Vector3 ToEulerYXZ() const;
	
	Quaternion Inverse() const
	{ 
		return Quaternion(Vector4(-1.f * _x, -1.f * _y, -1.f * _z, _w));
	}

	Quaternion& operator=(const Quaternion& other)
	{
		_data = other._data;

		return *this;
	}

	Quaternion operator*(const Quaternion& other) const;

	Vector3 Transform(const Vector3& point) const
	{
		Quaternion result = Inverse() * Quaternion(Vector4(point.x, point.y, point.z, 0.f)) * *this;

		return Vector3(result._data.x, result._data.y, result._data.z);
	}

	Vector3 GetForwardVector() const
	{
		return Transform(Vector3(0.f, 0.f, 1.f));
	}

	Vector3 GetRightVector() const
	{
		return Transform(Vector3(1.f, 0.f, 0.f));
	}

	Vector3 GetUpVector() const
	{
		return Transform(Vector3(0.f, 1.f, 0.f));
	}

	static Quaternion Lerp(const Quaternion& from, const Quaternion& to, float alpha);

	bool operator!=(const Quaternion& other) const
	{
		return _data != other._data;
	}

	bool operator==(const Quaternion& other) const
	{
		return _data == other._data;
	}
};
