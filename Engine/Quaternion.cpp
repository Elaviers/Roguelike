#include "Quaternion.hpp"
#include "Maths.hpp"

Quaternion::Quaternion(const Vector3& axis, float angle)
{
	Vector3 a = axis * Maths::SineDegrees(angle / 2.f);

	_x = a[0];
	_y = a[1];
	_z = a[2];
	_w = Maths::CosineDegrees(angle / 2.f);
}

Quaternion Quaternion::FromEulerZYX(const Vector3& euler)
{
	Quaternion pitch = Quaternion(VectorMaths::V3X, euler[0]);
	Quaternion yaw = Quaternion(VectorMaths::V3Y, -euler[1]);
	Quaternion roll = Quaternion(VectorMaths::V3Z, euler[2]);

	return pitch * yaw * roll;
}

Quaternion Quaternion::FromEulerYXZ(const Vector3& euler)
{
	Quaternion pitch = Quaternion(VectorMaths::V3X, euler[0]);
	Quaternion yaw = Quaternion(VectorMaths::V3Y, -euler[1]);
	Quaternion roll = Quaternion(VectorMaths::V3Z, euler[2]);

	return roll * pitch * yaw;
}

Vector3 Quaternion::ToEuler() const
{
	//ZXY
	return Vector3(
		Maths::ArcSineDegrees(2.f * (_y * _z + _x * _w)),
		Maths::ArcTangentDegrees2(2.f * (_x * _z - _y * _w), 1.f - 2.f * (_x * _x + _y * _y)),
		Maths::ArcTangentDegrees2(-2.f * (_x * _y - _z * _w), 1.f - 2.f * (_x * _x + _z * _z))
	);
}

Mat4 Quaternion::ToMatrix() const
{
	RETURNMAT4(
		1.f - 2.f * (_y * _y + _z * _z),	2.f * (_x * _y - _z * _w),			2.f * (_x * _z + _y * _w),			0.f,
		2.f * (_x * _y + _z * _w),			1.f - 2.f * (_x * _x + _z * _z),	2.f * (_y * _z - _x * _w),			0.f,
		2.f * (_x * _z - _y * _w),			2.f * (_y * _z + _x * _w),			1.f - 2.f * (_x * _x + _y * _y),	0.f,
		0.f,								0.f,								0.f,								1.f);
}

Quaternion Quaternion::operator*(const Quaternion & q) const
{
	return Quaternion(
		Vector4(
		_w * q._x + _x * q._w + _y * q._z - _z * q._y,
		_w * q._y - _x * q._z + _y * q._w + _z * q._x,
		_w * q._z + _x * q._y - _y * q._x + _z * q._w,
		_w * q._w - _x * q._x - _y * q._y - _z * q._z
		)
	);
}

Quaternion Quaternion::Lerp(const Quaternion& from, const Quaternion& to, float alpha)
{
	Quaternion q(Maths::Lerp(from._data, to._data, alpha));
	q._data.Normalise();

	return q;
}
