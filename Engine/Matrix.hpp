#pragma once
#include "Vector.hpp"
#include "Vector4.hpp"

class Quaternion;

#define RETURNMAT4(A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P) \
	{float data[4][4] = {A,B,C,D,E,F,G,H,I,J,K,L,M,N,O,P}; return Mat4(data);}

template <typename T, int SIZE>
class SquareMatrix
{
private:
	T _data[SIZE][SIZE];

	T Dot(const SquareMatrix &other, int r, int c) const
	{
		T result = 0;

		for (int i = 0; i < SIZE; ++i)
			result += _data[r][i] * other._data[i][c];

		return result;
	}

public:
	SquareMatrix() : _data{}
	{
		for (unsigned int r = 0; r < SIZE; ++r)
			for (unsigned int c = 0; c < SIZE; ++c)
				if (r == c)
					_data[r][c] = 1;
				else
					_data[r][c] = 0;
	}
	
#pragma warning(suppress: 26495) //All of data is initialised here; False positive
	SquareMatrix(T data[SIZE][SIZE])
	{
		int sizeSq = SIZE * SIZE;
		for (int i = 0; i < sizeSq; ++i)
			_data[0][i] = data[0][i];
	}

	~SquareMatrix() {}

	SquareMatrix& operator=(const SquareMatrix& other)
	{
		int sizeSq = SIZE * SIZE;
		for (int i = 0; i < sizeSq; ++i)
			_data[0][i] = other._data[0][i];

		return *this;
	}
	
	inline			T* operator[](int index)		{ return _data[index]; }
	const	T* operator[](int index) const	{ return _data[index]; }

	friend SquareMatrix operator*(const SquareMatrix &a, const SquareMatrix &b);

	SquareMatrix& operator*=(const SquareMatrix& other) { return *this = *this * other; }

	SquareMatrix& operator*=(const T &other)
	{
		int sizeSq = SIZE * SIZE;
		for (int i = 0; i < sizeSq; ++i)
			_data[0][i] *= other;

		return *this;
	}

	friend Vector3 operator*(const Vector3&, const SquareMatrix<float, 4>&);
	friend Vector4 operator*(const Vector4&, const SquareMatrix<float, 4>&);

	friend SquareMatrix<float, 3> Inverse(const SquareMatrix<float, 3>&);
	friend SquareMatrix<float, 4> Inverse(const SquareMatrix<float, 4>&);
};

typedef SquareMatrix<float, 3> Mat3;
typedef SquareMatrix<float, 4> Mat4;

template<typename T, int SIZE>
SquareMatrix<T, SIZE> operator*(const SquareMatrix<T, SIZE> &other)
{
	SquareMatrix<T, SIZE> result;

	for (int r = 0; r < SIZE; ++r)
		for (int c = 0; c < SIZE; ++c)
			result[r][c] = Dot(other, r, c);

	return result;
}

Mat4 operator*(const Mat4 &a, const Mat4 &b);

Vector4 operator*(const Vector4&, const Mat4&);

inline Vector3 operator*(const Vector3 &v, const Mat4 &m)
{
	Vector4 v4 = Vector4(v[0], v[1], v[2], 1.f) * m;
	return Vector3(v4[0], v4[1], v4[2]);
}

Vector4 operator*(const Vector4&, const Mat4&);

Mat3 Inverse(const Mat3&);

namespace Matrix
{
	Mat4 Identity();

	Mat4 Translation(const Vector3 &translation);
	Mat4 RotationX(float angle);
	Mat4 RotationY(float angle);
	Mat4 RotationZ(float angle);
	Mat4 Scale(const Vector3 &scale);

	Mat4 Transformation(const Vector3 &translation, const Vector3 &rotation, const Vector3 &scale);
	Mat4 Transformation(const Vector3 &translation, const Quaternion &rotation, const Vector3 &scale);

	Mat4 Ortho(float left, float right, float bottom, float top, float near, float far, float scale = 1.f);

	Mat4 Perspective(float fieldOfView, float near, float far, float aspectRatio);
}
