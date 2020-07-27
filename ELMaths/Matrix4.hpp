#pragma once
#include "Vector3.hpp"
#include "Vector4.hpp"
#include "Quaternion.hpp"
#include <ELCore/ByteReader.hpp>
#include <ELCore/ByteWriter.hpp>
#include <ELCore/String.hpp>

template <typename T>
union Matrix4T
{
private:
	T _data[16];

public:
	struct
	{
		T m00; T m01; T m02; T m03;
		T m10; T m11; T m12; T m13;
		T m20; T m21; T m22; T m23;
		T m30; T m31; T m32; T m33;
	};

	Matrix4T(
		const T& m00, const T& m01, const T& m02, const T& m03,
		const T& m10, const T& m11, const T& m12, const T& m13,
		const T& m20, const T& m21, const T& m22, const T& m23,
		const T& m30, const T& m31, const T& m32, const T& m33
	)
		: m00(m00), m01(m01), m02(m02), m03(m03),
		m10(m10), m11(m11), m12(m12), m13(m13),
		m20(m20), m21(m21), m22(m22), m23(m23),
		m30(m30), m31(m31), m32(m32), m33(m33) {}

	Matrix4T() : Matrix4T(
		(T)1, (T)0, (T)0, (T)0, 
		(T)0, (T)1, (T)0, (T)0, 
		(T)0, (T)0, (T)1, (T)0, 
		(T)0, (T)0, (T)0, (T)1) {}

	T* operator[](int row) { return _data + row * 4; }
	const T* operator[](int row) const { return _data + row * 4; }

	Matrix4T& operator+=(const Matrix4T& other)
	{
		m00 += other.m00;	m01 += other.m01; m02 += other.m02; m03 += other.m03;
		m10 += other.m10;	m11 += other.m11; m12 += other.m12; m13 += other.m13;
		m20 += other.m20;	m21 += other.m21; m22 += other.m22; m23 += other.m23;
		m30 += other.m30;	m31 += other.m31; m32 += other.m32; m33 += other.m33;
		return *this;
	}

	Matrix4T& operator-=(const Matrix4T& other)
	{
		m00 -= other.m00;	m01 -= other.m01; m02 -= other.m02; m03 -= other.m03;
		m10 -= other.m10;	m11 -= other.m11; m12 -= other.m12; m13 -= other.m13;
		m20 -= other.m20;	m21 -= other.m21; m22 -= other.m22; m23 -= other.m23;
		m30 -= other.m30;	m31 -= other.m31; m32 -= other.m32; m33 -= other.m33;
		return *this;
	}

	Matrix4T& operator*=(const T& scalar)
	{
		m00 *= scalar; m01 *= scalar; m02 *= scalar; m03 *= scalar;
		m10 *= scalar; m11 *= scalar; m12 *= scalar; m13 *= scalar;
		m20 *= scalar; m21 *= scalar; m22 *= scalar; m23 *= scalar;
		m30 *= scalar; m31 *= scalar; m32 *= scalar; m33 *= scalar;
		return *this;
	}

	Matrix4T operator*(const T& scalar) const
	{
		return Matrix4T(
			m00 * scalar, m01 * scalar, m02 * scalar, m03 * scalar,
			m10 * scalar, m11 * scalar, m12 * scalar, m13 * scalar,
			m20 * scalar, m21 * scalar, m22 * scalar, m23 * scalar,
			m30 * scalar, m31 * scalar, m32 * scalar, m33 * scalar
		);
	}

	Matrix4T operator*(const Matrix4T& other) const
	{
		return Matrix4T(
			m00 * other.m00 + m01 * other.m10 + m02 * other.m20 + m03 * other.m30, m00 * other.m01 + m01 * other.m11 + m02 * other.m21 + m03 * other.m31, m00 * other.m02 + m01 * other.m12 + m02 * other.m22 + m03 * other.m32, m00 * other.m03 + m01 * other.m13 + m02 * other.m23 + m03 * other.m33,
			m10 * other.m00 + m11 * other.m10 + m12 * other.m20 + m13 * other.m30, m10 * other.m01 + m11 * other.m11 + m12 * other.m21 + m13 * other.m31, m10 * other.m02 + m11 * other.m12 + m12 * other.m22 + m13 * other.m32, m10 * other.m03 + m11 * other.m13 + m12 * other.m23 + m13 * other.m33,
			m20 * other.m00 + m21 * other.m10 + m22 * other.m20 + m23 * other.m30, m20 * other.m01 + m21 * other.m11 + m22 * other.m21 + m23 * other.m31, m20 * other.m02 + m21 * other.m12 + m22 * other.m22 + m23 * other.m32, m20 * other.m03 + m21 * other.m13 + m22 * other.m23 + m23 * other.m33,
			m30 * other.m00 + m31 * other.m10 + m32 * other.m20 + m33 * other.m30, m30 * other.m01 + m31 * other.m11 + m32 * other.m21 + m33 * other.m31, m30 * other.m02 + m31 * other.m12 + m32 * other.m22 + m33 * other.m32, m30 * other.m03 + m31 * other.m13 + m32 * other.m23 + m33 * other.m33
		);
	}

	void MultiplyRow(int row, float x)
	{
		int start = row * 4;
		_data[start] *= x;
		_data[start + 1] *= x;
		_data[start + 2] *= x;
		_data[start + 3] *= x;
	}

	void MultiplyColumn(int column, float x)
	{
		_data[column] *= x;
		_data[column + 4] *= x;
		_data[column + 8] *= x;
		_data[column + 12] *= x;
	}

	void Write(ByteWriter& writer) const
	{
		for (int i = 0; i < 16; ++i)
			writer.Write_float(_data[i]);
	}

	void Read(ByteReader& reader)
	{
		for (int i = 0; i < 16; ++i)
			_data[i] = reader.Read_float();
	}

	String ToString(const char colSeperator = ", ", const char rowSeperator = "\n") const
	{
		return String::Concat(
			m00, colSeperator, m01, colSeperator, m02, colSeperator, m03, rowSeperator,
			m10, colSeperator, m11, colSeperator, m12, colSeperator, m13, rowSeperator,
			m20, colSeperator, m21, colSeperator, m22, colSeperator, m23, rowSeperator,
			m30, colSeperator, m31, colSeperator, m32, colSeperator, m33
		);
	}

	static const Matrix4T& Empty() 
	{ 
		static Matrix4T empty((T)0, (T)0, (T)0, (T)0, (T)0, (T)0, (T)0, (T)0, (T)0, (T)0, (T)0, (T)0, (T)0, (T)0, (T)0, (T)0);
		return empty;
	}

	static const Matrix4T& Identity()
	{
		static Matrix4T identity;
		return identity;
	}

	static Matrix4T RotationX(const T& degrees)
	{
		T sin = Maths::SineDegrees(degrees);
		T cos = Maths::CosineDegrees(degrees);

		return Matrix4T(
			(T)1, (T)0, (T)0, (T)0,
			(T)0, cos, -sin, (T)0,
			(T)0, sin, cos, (T)0,
			(T)0, (T)0, (T)0, (T)1
		);
	}

	static Matrix4T RotationY(const T& degrees)
	{
		T sin = Maths::SineDegrees(degrees);
		T cos = Maths::CosineDegrees(degrees);

		return Matrix4T(
			cos, (T)0, -sin, (T)0,
			(T)0, (T)1, (T)0, (T)0,
			sin, (T)0, cos, (T)0,
			(T)0, (T)0, (T)0, (T)1
		);
	}

	static Matrix4T RotationZ(const T& degrees)
	{
		T sin = Maths::SineDegrees(degrees);
		T cos = Maths::CosineDegrees(degrees);

		return Matrix4T(
			cos, -sin, (T)0, (T)0,
			sin, cos, (T)0, (T)0,
			(T)0, (T)0, (T)1, (T)0,
			(T)0, (T)0, (T)0, (T)1
		);
	}

	static Matrix4T Scale(const Vector3T<T>& s)
	{
		return Matrix4T(
			s.x, (T)0, (T)0, (T)0,
			(T)0, s.y, (T)0, (T)0,
			(T)0, (T)0, s.z, (T)0,
			(T)0, (T)0, (T)0, (T)1
		);
	}

	static Matrix4T Translation(const Vector3T<T>& t)
	{
		return Matrix4T(
			(T)1, (T)0, (T)0, (T)0,
			(T)0, (T)1, (T)0, (T)0,
			(T)0, (T)0, (T)1, (T)0,
			t.x, t.y, t.z, (T)1
		);
	}

	static Matrix4T Rotation(const Quaternion& r)
	{
		float x = r.GetData().x;
		float y = r.GetData().y;
		float z = r.GetData().z;
		float w = r.GetData().w;
		return Matrix4T(
			1.f - 2.f * (y * y + z * z), 2.f * (x * y - z * w), 2.f * (x * z + y * w), 0.f,
			2.f * (x * y + z * w), 1.f - 2.f * (x * x + z * z), 2.f * (y * z - x * w), 0.f,
			2.f * (x * z - y * w), 2.f * (y * z + x * w), 1.f - 2.f * (x * x + y * y), 0.f,
			0.f, 0.f, 0.f, 1.f);
	}

	//euler angles are ZXY
	static Matrix4T Transformation(const Vector3T<T>& translation, const Vector3T<T>& euler, const Vector3T<T>& scale)
	{
		//Boiled-down version of Scale(scale) * RotationZ(euler.z) * RotationX(euler.x) * RotationY(euler.y) * Translation(translation)
		float sinRX = Maths::SineDegrees(euler.x);
		float nsinRX = -sinRX;
		float sinRY = Maths::SineDegrees(euler.y);
		float nsinRY = -sinRY;
		float sinRZ = Maths::SineDegrees(euler.z);
		float cosRX = Maths::CosineDegrees(euler.x);
		float cosRY = Maths::CosineDegrees(euler.y);
		float cosRZ = Maths::CosineDegrees(euler.z);

		float f1 = scale.x * cosRZ;
		float f2 = scale.x * -sinRZ;
		float f3 = scale.y * sinRZ;
		float f4 = scale.y * cosRZ;
		float f5 = scale.z * cosRX;

		return Matrix4T(
			f1 * cosRY + f2 * nsinRX * sinRY, f2 * cosRX, f1 * nsinRY + f2 * nsinRX * cosRY, 0.f,
			f3 * cosRY + f4 * nsinRX * sinRY, f4 * cosRX, f3 * nsinRY + f4 * nsinRX * cosRY, 0.f,
			f5 * sinRY, scale.z * sinRX, f5 * cosRY, 0.f,
			translation.x, translation.y, translation.z, 1.f
		);
	}

	static Matrix4T Transformation(const Vector3T<T>& translation, const Quaternion& rotation, const Vector3T<T>& scale)
	{
		return Scale(scale) * Rotation(rotation) * Translation(translation);
	}

	static Matrix4T ProjectionOrtho(const T& left, const T& right, const T& bottom, const T& top, const T& n, const T& f, const T& scale) 
	{
		return Matrix4T(
			(T)2 * scale / (right - left), (T)0, (T)0, (T)0,
			(T)0, (T)2 * scale / (top - bottom), (T)0, (T)0,
			(T)0, (T)0, (T)2 / (f - n), (T)0,
			(-right - left) / (right - left), (-top - bottom) / (top - bottom), (-f - n) / (f - n), (T)1
		);
	}

	static Matrix4T ProjectionPerspective(const T& vFovRadians, const T& n, const T& f, const T& aspectRatio)
	{
		T tan = Maths::Tangent(vFovRadians / (T)2);

		return Matrix4T(
			(T)1 / aspectRatio * tan, (T)0, (T)0, (T)0,
			(T)0, (T)1 / tan, (T)0, (T)0,
			(T)0, (T)0, f / (f - n), (T)1,
			(T)0, (T)0, -(f * n) / (f - n), (T)0
		);
	}
};

typedef Matrix4T<float> Matrix4;

inline Vector4 operator*(const Vector4& v, const Matrix4& m)
{
	return Vector4(
		v.x * m.m00 + v.y * m.m10 + v.z * m.m20 + v.w * m.m30,
		v.x * m.m01 + v.y * m.m11 + v.z * m.m21 + v.w * m.m31,
		v.x * m.m02 + v.y * m.m12 + v.z * m.m22 + v.w * m.m32,
		v.x * m.m03 + v.y * m.m13 + v.z * m.m23 + v.w * m.m33);
}
