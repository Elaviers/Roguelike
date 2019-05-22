#include "Vector.hpp"
#include "Matrix.hpp"

using namespace Maths;

namespace VectorMaths
{
	const Vector3 V3X(1, 0, 0);
	const Vector3 V3Y(0, 1, 0);
	const Vector3 V3Z(0, 0, 1);

	Vector3 GetForwardVector(const Vector3 &rotation)
	{
		//cos pitch * sin yaw, sin pitch, cos pitch * cos yaw
		float cosPitch = CosineDegrees(rotation[0]);

		return Vector3(cosPitch * SineDegrees(rotation[1]), SineDegrees(rotation[0]), cosPitch * CosineDegrees(rotation[1]));
	}

	Vector3 GetRightVector(const Vector3 &rotation)
	{
		//cos roll * cos yaw, -sin roll, cos roll * -sin yaw
		float cosRoll = CosineDegrees(rotation[2]);

		return Vector3(cosRoll * CosineDegrees(rotation[1]), -SineDegrees(rotation[2]), cosRoll * -SineDegrees(rotation[1]));
	}

	Vector3 GetUpVector(const Vector3 &rotation)
	{
		//sin roll * cos yaw + -sin pitch * sin yaw, cos pitch * cos roll, -sin pitch * cos yaw + sin roll * sin yaw
		float sinYaw = SineDegrees(rotation[1]);
		float cosYaw = CosineDegrees(rotation[1]);
		float nSinPitch = -SineDegrees(rotation[0]);
		float sinRoll = SineDegrees(rotation[2]);

		return Vector3(sinRoll * cosYaw + nSinPitch * sinYaw, CosineDegrees(rotation[0]) * CosineDegrees(rotation[2]), nSinPitch * cosYaw + sinRoll * sinYaw);
	}

	Vector3 Rotate(const Vector3 &vector, const Vector3 &rotation)
	{
		return vector * Matrix::RotationZ(rotation[2]) * Matrix::RotationX(rotation[0]) * Matrix::RotationY(rotation[1]);
	}
}
