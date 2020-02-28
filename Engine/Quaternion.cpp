#include "Quaternion.hpp"
#include "Maths.hpp"

Quaternion::Quaternion(const Vector3& axis, float angle)
{
	Vector3 a = axis * Maths::SineDegrees(angle / 2.f);

	x = a[0];
	y = a[1];
	_z = a[2];
	w = Maths::CosineDegrees(angle / 2.f);
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
		Maths::ArcSineDegrees(2.f * (y * _z + x * w)),
		Maths::ArcTangentDegrees2(2.f * (x * _z - y * w), 1.f - 2.f * (x * x + y * y)),
		Maths::ArcTangentDegrees2(-2.f * (x * y - _z * w), 1.f - 2.f * (x * x + _z * _z))
	);
}

Mat4 Quaternion::ToMatrix() const
{
	RETURNMAT4(
		1.f - 2.f * (y * y + _z * _z),	2.f * (x * y - _z * w),			2.f * (x * _z + y * w),			0.f,
		2.f * (x * y + _z * w),			1.f - 2.f * (x * x + _z * _z),	2.f * (y * _z - x * w),			0.f,
		2.f * (x * _z - y * w),			2.f * (y * _z + x * w),			1.f - 2.f * (x * x + y * y),	0.f,
		0.f,								0.f,								0.f,								1.f);
}

Quaternion Quaternion::operator*(const Quaternion & q) const
{
	return Quaternion(
		Vector4(
		w * q.x + x * q.w + y * q._z - _z * q.y,
		w * q.y - x * q._z + y * q.w + _z * q.x,
		w * q._z + x * q.y - y * q.x + _z * q.w,
		w * q.w - x * q.x - y * q.y - _z * q._z
		)
	);
}

Quaternion Quaternion::Lerp(const Quaternion& from, const Quaternion& to, float alpha)
{
	Quaternion q(Maths::Lerp(from._data, to._data, alpha));
	q._data.Normalise();

	return q;
}
