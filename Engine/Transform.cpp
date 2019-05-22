#include "Transform.hpp"
#include "Maths.hpp"

using namespace Maths;

void Transform::_MakeTransformationMatrix(Mat4 &matrix) const
{
	matrix = Matrix::Transformation(_position, _rotation.GetQuat(), _scale);
}

void Transform::_MakeInverseTransformationMatrix(Mat4 &matrix) const
{
	matrix = 
		Matrix::Translation(-1.f * _position) * 
		_rotation.GetQuat().Inverse().ToMatrix() *
		Matrix::Scale(1.f / _scale);
}

Mat4 Transform::GetTransformationMatrix() const
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

Mat4 Transform::GetInverseTransformationMatrix() const
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
	buffer.Write_vector3(_rotation.GetEuler());
	buffer.Write_vector3(_scale);
}

void Transform::ReadFromBuffer(BufferIterator<byte> &buffer)
{
	_position = buffer.Read_vector3();
	_rotation = Quaternion(buffer.Read_vector3());
	_scale = buffer.Read_vector3();
}

Transform& Transform::operator*=(const Transform &other)
{
	_position = _position * other.GetTransformationMatrix();
	_rotation = other._rotation * _rotation;
	_scale *= other._scale;

	return *this;
}
