#include "Transform.h"
#include "Maths.h"

Mat4 Transform::MakeTransformationMatrix()
{
	return
		Matrix::Scale(_scale) *
		Matrix::RotationZ(_rotation[2]) * Matrix::RotationX(_rotation[0]) * Matrix::RotationY(_rotation[1]) *
		Matrix::Translation(_position);
}

Mat4 Transform::MakeInverseTransformationMatrix()
{
	return
		Matrix::Translation(-1.f * _position) *
		Matrix::RotationY(-_rotation[1]) * Matrix::RotationX(-_rotation[0]) * Matrix::RotationZ(-_rotation[2]) *
		Matrix::Scale(1.f / _scale);
}

Vector3 Transform::GetForwardVector()
{
	//cos pitch * sin yaw, sin pitch, cos pitch * cos yaw
	float cosPitch = CosineDegrees(_rotation[0]);

	return Vector3(cosPitch * SineDegrees(_rotation[1]), SineDegrees(_rotation[0]), cosPitch * CosineDegrees(_rotation[1]));
}

Vector3 Transform::GetRightVector()
{
	//cos roll * cos yaw, -sin roll, cos roll * -sin yaw
	float cosRoll = CosineDegrees(_rotation[2]);

	return Vector3(cosRoll * CosineDegrees(_rotation[1]), -SineDegrees(_rotation[2]), cosRoll * -SineDegrees(_rotation[1]));
}

Vector3 Transform::GetUpVector()
{
	//sin roll * cos yaw + -sin pitch * sin yaw, cos pitch * cos roll, -sin pitch * cos yaw + sin roll * sin yaw
	float sinYaw = SineDegrees(_rotation[1]);
	float cosYaw = CosineDegrees(_rotation[1]);
	float nSinPitch = -SineDegrees(_rotation[0]);
	float sinRoll = SineDegrees(_rotation[2]);

	return Vector3(sinRoll * cosYaw + nSinPitch * sinYaw, CosineDegrees(_rotation[0]) * CosineDegrees(_rotation[2]), nSinPitch * cosYaw + sinRoll * sinYaw);
}
