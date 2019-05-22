#include "Matrix.h"
#include "Maths.h"
#include "Rotation.h"
#include "Quaternion.h"

using namespace Maths;

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

	Mat4 Transformation(const Vector3 &translation, const Vector3 &rotation, const Vector3 &scale)
	{
		float sinRX = SineDegrees(rotation[0]);
		float nsinRX = -sinRX;
		float sinRY = SineDegrees(rotation[1]);
		float nsinRY = -sinRY;
		float sinRZ = SineDegrees(rotation[2]);
		float cosRX = CosineDegrees(rotation[0]);
		float cosRY = CosineDegrees(rotation[1]);
		float cosRZ = CosineDegrees(rotation[2]);

		float f1 = scale[0] * cosRZ;
		float f2 = scale[0] * -sinRZ;
		float f3 = scale[1] * sinRZ;
		float f4 = scale[1] * cosRZ;
		float f5 = scale[2] * cosRX;

		RETURNMAT4(
			f1 * cosRY + f2 * nsinRX * sinRY, 			f2 * cosRX,			f1 * nsinRY + f2 * nsinRX * cosRY,					0.f,
			f3 * cosRY + f4 * nsinRX *sinRY,			f4 * cosRX,			f3 * nsinRY + f4 * nsinRX * cosRY, 					0.f,
			f5 * sinRY,									scale[2] * sinRX,	f5 * cosRY,											0.f,
			translation[0],								translation[1],		translation[2],										1.f
		);
	}

	Mat4 Transformation(const Vector3& translation, const Quaternion& rotation, const Vector3& scale)
	{
		return Scale(scale) * rotation.ToMatrix() * Translation(translation);
	}

	Mat4 Ortho(float width, float height, float near, float far, float scale)
	{
		RETURNMAT4(
			2.f * scale / width,	0.f,					0.f,							0.f,
			0.f,					2.f * scale / height,	0.f,							0.f,
			0.f,					0.f,					2.f / (far - near),				0.f,
			0.f,					0.f,					(-far - near) / (far - near),	1.f
		);
	}

	Mat4 Perspective(float fieldOfView, float near, float far, float aspectRatio)
	{
		float tan = TangentDegrees(fieldOfView / 2.f);
		
		RETURNMAT4(
			1.f / aspectRatio * tan,	0.f,		0.f,							0.f,
			0.f,						1.f / tan,	0.f,							0.f,
			0.f,						0.f,		far / (far - near),				1.f,
			0.f,						0.f,		-(far * near) / (far - near),	0.f
		);
	}
}

#define SHUFFLE_PARAM(X, Y, Z, W) (X) | (Y << 2) | (Z << 4) | (W << 6)
#define MM_SHUFFLE_X(v) _mm_shuffle_ps(v, v, SHUFFLE_PARAM(0, 0, 0, 0))
#define MM_SHUFFLE_Y(v) _mm_shuffle_ps(v, v, SHUFFLE_PARAM(1, 1, 1, 1))
#define MM_SHUFFLE_Z(v) _mm_shuffle_ps(v, v, SHUFFLE_PARAM(2, 2, 2, 2))
#define MM_SHUFFLE_W(v) _mm_shuffle_ps(v, v, SHUFFLE_PARAM(3, 3, 3, 3))

inline void VectorTimesMatrixSIMD(float out[4], __m128 simd, __m128 row1, __m128 row2, __m128 row3, __m128 row4)
{
	row1 = _mm_mul_ps(MM_SHUFFLE_X(simd), row1);
	row2 = _mm_mul_ps(MM_SHUFFLE_Y(simd), row2);
	row3 = _mm_mul_ps(MM_SHUFFLE_Z(simd), row3);
	row4 = _mm_mul_ps(MM_SHUFFLE_W(simd), row4);

	simd = _mm_add_ps(row1, row2);
	simd = _mm_add_ps(simd, row3);
	simd = _mm_add_ps(simd, row4);
	_mm_store_ps(out, simd);
}

Vector4 operator*(const Vector4 &v, const Mat4 &m)
{
	Vector4 result;
	VectorTimesMatrixSIMD(&result[0], v.LoadSIMD(), _mm_load_ps(m[0]), _mm_load_ps(m[1]), _mm_load_ps(m[2]), _mm_load_ps(m[3]));
	return result;
}

inline void MultiplyRow(float out[4], const float row[4], const Mat4 &m)
{
	return VectorTimesMatrixSIMD(out, _mm_load_ps(row), _mm_load_ps(m[0]), _mm_load_ps(m[1]), _mm_load_ps(m[2]), _mm_load_ps(m[3]));
}

Mat4 operator*(const Mat4 &a, const Mat4 &b)
{
	Mat4 result;
	MultiplyRow(result[0], a._data[0], b);
	MultiplyRow(result[1], a._data[1], b);
	MultiplyRow(result[2], a._data[2], b);
	MultiplyRow(result[3], a._data[3], b);
	return result;
}

Mat3 Inverse(const Mat3 &matrix)
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
