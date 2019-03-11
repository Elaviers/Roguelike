#include "Transform.h"
#include "Maths.h"

using namespace Maths;

void Transform::_MakeTransformationMatrix(Mat4 &matrix) const
{
	matrix = Matrix::Transformation(_position, _rotation, _scale);
}

void Transform::_MakeInverseTransformationMatrix(Mat4 &matrix) const
{
	matrix = 
		Matrix::Translation(-1.f * _position) * 
		Matrix::RotationY(-_rotation[1]) * 
		Matrix::RotationX(-_rotation[0]) * 
		Matrix::RotationZ(-_rotation[2]) * 
		Matrix::Scale(1.f / _scale);
}

const Mat4& Transform::GetTransformationMatrix() const
{
	//todo: hm.
	Transform *disgusting = const_cast<Transform*>(this);

	if (_matrixStatus != MAT_TRANSFORM)
	{
		_MakeTransformationMatrix(disgusting->_matrix);
		disgusting->_matrixStatus = MAT_TRANSFORM;
	}

	return _matrix;
}

const Mat4& Transform::GetInverseTransformationMatrix() const
{
	Transform *disgusting = const_cast<Transform*>(this);

	if (_matrixStatus != MAT_INVERSETRANSFORM)
	{
		_MakeInverseTransformationMatrix(disgusting->_matrix);
		disgusting->_matrixStatus = MAT_INVERSETRANSFORM;
	}

	return _matrix;
}

Mat4 Transform::MakeTransformationMatrix() const
{
	if (_matrixStatus == MAT_TRANSFORM)
		return _matrix;

	Mat4 m;
	_MakeTransformationMatrix(m);
	return m;
}

Mat4 Transform::MakeInverseTransformationMatrix() const
{
	if (_matrixStatus == MAT_INVERSETRANSFORM)
		return _matrix;

	Mat4 m;
	_MakeInverseTransformationMatrix(m);
	return m;
}

void Transform::WriteToBuffer(BufferIterator<byte> &buffer) const
{
	buffer.Write_vector3(_position);
	buffer.Write_vector3(_rotation);
	buffer.Write_vector3(_scale);
}

void Transform::ReadFromBuffer(BufferIterator<byte> &buffer)
{
	_position = buffer.Read_vector3();
	_rotation = buffer.Read_vector3();
	_scale = buffer.Read_vector3();
}

Transform& Transform::operator*=(const Transform &other)
{
	//_position = other._position + other._scale * (other.GetForwardVector() * _position[0] + other.GetUpVector() * _position[1] + other.GetRightVector() * _position[2]);
	_position = _position * other.GetTransformationMatrix();
	_rotation += other._rotation;
	_scale *= other._scale;

	return *this;
}
