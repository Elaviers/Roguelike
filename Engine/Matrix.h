#pragma once
#include "Vector.h"

template <typename T, int SIZE>
class SquareMatrix
{
private:
	T _data[SIZE][SIZE];

	inline T Dot(const SquareMatrix &other, int r, int c) const
	{
		T result = 0;

		for (int i = 0; i < SIZE; ++i)
			result += _data[r][i] * other._data[i][c];

		return result;
	}

public:
	SquareMatrix() 
	{
		for (unsigned int r = 0; r < SIZE; ++r)
			for (unsigned int c = 0; c < SIZE; ++c)
				if (r == c)
					_data[r][c] = 1;
				else
					_data[r][c] = 0;
	}

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
	inline const	T* operator[](int index) const	{ return _data[index]; }
	
	SquareMatrix operator*(const SquareMatrix &other) const
	{
		SquareMatrix result;

		for (int r = 0; r < SIZE; ++r)
			for (int c = 0; c < SIZE; ++c)
				result[r][c] = Dot(other, r, c);

		return result;
	}

	SquareMatrix& operator*=(const SquareMatrix& other)
	{
		*this = *this * other;
		return *this;
	}

	SquareMatrix& operator*=(const T &other)
	{
		int sizeSq = SIZE * SIZE;
		for (int i = 0; i < sizeSq; ++i)
			_data[0][i] *= other;

		return *this;
	}

	friend Vector3 operator*(const Vector3&, const SquareMatrix<float, 4>&);

	friend SquareMatrix<float, 3> Inverse(const SquareMatrix<float, 3>&);
	friend SquareMatrix<float, 4> Inverse(const SquareMatrix<float, 4>&);
};

typedef SquareMatrix<float, 3> Mat3;
typedef SquareMatrix<float, 4> Mat4;

namespace Matrix
{
	Mat4 Identity();

	Mat4 Translation(const Vector3 &translation);
	Mat4 RotationX(float angle);
	Mat4 RotationY(float angle);
	Mat4 RotationZ(float angle);
	Mat4 Scale(const Vector3 &scale);

	Mat4 Ortho(float width, float height, float near, float far, float scale = 1.f);
	Mat4 Perspective(float fieldOfView, float near, float far, float aspectRatio);
}

Vector3 operator*(const Vector3&, const SquareMatrix<float, 4>&);

SquareMatrix<float, 3> Inverse(const SquareMatrix<float, 3>&);
