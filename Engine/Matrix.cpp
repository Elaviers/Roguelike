#include "Matrix.h"
#include "Maths.h"

#define RETURNMAT4(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P) \
	float data[4][4] = {A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P}; return Mat4(data)

namespace Matrix
{
	Mat4 Identity()
	{
		RETURNMAT4(
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f
		);
	}

	Mat4 Translation(const Vector3 &t)
	{
		RETURNMAT4(
			1.f, 0.f, 0.f, 0.f,
			0.f, 1.f, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			t[0],t[1],t[2],1.f
		);
	}

	Mat4 RotationX(float angle)
	{
		float sin = SineDegrees(angle);
		float cos = CosineDegrees(angle);

		RETURNMAT4(
			1.f, 0.f, 0.f, 0.f,
			0.f, cos, -sin, 0.f,
			0.f, sin, cos, 0.f,
			0.f, 0.f, 0.f, 1.f
		);
	}

	Mat4 RotationY(float angle)
	{
		float sin = SineDegrees(angle);
		float cos = CosineDegrees(angle);

		RETURNMAT4(
			cos, 0.f, -sin, 0.f,
			0.f, 1.f, 0.f, 0.f,
			sin, 0.f, cos, 0.f,
			0.f, 0.f, 0.f, 1.f
		);
	}

	Mat4 RotationZ(float angle)
	{
		float sin = SineDegrees(angle);
		float cos = CosineDegrees(angle);

		RETURNMAT4(
			cos, -sin, 0.f, 0.f,
			sin, cos, 0.f, 0.f,
			0.f, 0.f, 1.f, 0.f,
			0.f, 0.f, 0.f, 1.f
		);
	}

	Mat4 Scale(const Vector3 &s)
	{
		RETURNMAT4(
			s[0],	0.f,	0.f,	0.f,
			0.f,	s[1],	0.f,	0.f,
			0.f,	0.f,	s[2],	0.f,
			0.f,	0.f,	0.f,	1.f
		);
	}

	Mat4 Ortho(float width, float height, float near, float far)
	{
		RETURNMAT4(
			2.f / width,	0.f,			0.f,							0.f,
			0.f,			2.f / height,	0.f,							0.f,
			0.f,			0.f,			2.f / (far - near),				0.f,
			-1.f,			-1.f,			(-far - near) / (far - near),	1.f
		);
	}

	Mat4 Perspective(float fieldOfView, float near, float far, float aspectRatio)
	{
		float scaleY = 1.f / TangentDegrees(fieldOfView / 2.f);
		float zDepth = (far - near);

		RETURNMAT4(
			scaleY / aspectRatio,	0.f,		0.f,					0.f,
			0.f,					scaleY,		0.f,					0.f,
			0.f,					0.f,		far / zDepth,			1.f,
			0.f,					0.f,		(-far * near) / zDepth,	0.f
		);
	}
}

SquareMatrix<float, 3> Inverse(const SquareMatrix<float, 3> &matrix)
{
	float r[3][3] =
	{
		matrix[1][1] * matrix[2][2] - matrix[1][2] * matrix[2][1],
		-(matrix[1][0] * matrix[2][2] - matrix[1][2] * matrix[0][2]),
		matrix[1][0] * matrix[2][1] - matrix[1][1] * matrix[2][0],
		-(matrix[0][1] * matrix[2][2] - matrix[0][2] * matrix[2][1]),
		matrix[0][0] * matrix[2][2] - matrix[0][2] * matrix[2][0],
		-(matrix[0][0] * matrix[2][1] - matrix[0][1] * matrix[2][0]),
		matrix[0][1] * matrix[1][2] - matrix[0][2] * matrix[1][1],
		-(matrix[0][0] * matrix[1][2] - matrix[0][2] * matrix[1][0]),
		matrix[0][0] * matrix[1][1] - matrix[0][1] * matrix[1][0]
	};

	Mat3 result(r);

	float determinant = matrix.Dot(result, 0, 0) + matrix.Dot(result, 1, 1) + matrix.Dot(result, 2, 2);
	result *= 1.f / determinant;

	return result;
}
