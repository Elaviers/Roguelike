#include "Transform.hpp"
#include "Maths.hpp"

using namespace Maths;

void Transform::_MakeTransformationMatrix(Matrix4 &matrix) const
{
	matrix = Matrix4::Transformation(_position, _rotation.GetQuat(), _scale);
}

void Transform::_MakeInverseTransformationMatrix(Matrix4 &matrix) const
{
	matrix = 
		Matrix4::Translation(-1.f * _position) *
		Matrix4::Rotation(_rotation.GetQuat().Inverse()) *
		Matrix4::Scale(1.f / _scale);
}

Matrix4 Transform::GetTransformationMatrix() const
{
	//todo: hm.
	Transform *disgusting = const_cast<Transform*>(this);

	if (_matrixStatus != EMatrixStatus::TRANSFORM)
	{
		_MakeTransformationMatrix(disgusting->_matrix);
		disgusting->_matrixStatus = EMatrixStatus::TRANSFORM;
	}

	return _matrix;
}

Matrix4 Transform::GetInverseTransformationMatrix() const
{
	Transform *disgusting = const_cast<Transform*>(this);

	if (_matrixStatus != EMatrixStatus::INVERSETRANSFORM)
	{
		_MakeInverseTransformationMatrix(disgusting->_matrix);
		disgusting->_matrixStatus = EMatrixStatus::INVERSETRANSFORM;
	}

	return _matrix;
}

Matrix4 Transform::MakeTransformationMatrix() const
{
	if (_matrixStatus == EMatrixStatus::TRANSFORM)
		return _matrix;

	Matrix4 m;
	_MakeTransformationMatrix(m);
	return m;
}

Matrix4 Transform::MakeInverseTransformationMatrix() const
{
	if (_matrixStatus == EMatrixStatus::INVERSETRANSFORM)
		return _matrix;

	Matrix4 m;
	_MakeInverseTransformationMatrix(m);
	return m;
}

void Transform::Write(ByteWriter &writer) const
{
	_position.Write(writer);
	_rotation.GetEuler().Write(writer);
	_scale.Write(writer);
}

void Transform::Read(ByteReader &buffer)
{
	_position.Read(buffer);
	_rotation = Quaternion(buffer.Read<Vector3>());
	_scale.Read(buffer);
	_Update();
}

Transform& Transform::operator*=(const Transform &other)
{
	_position = (Vector4(_position, 1.f) * other.GetTransformationMatrix()).GetXYZ();
	_rotation = _rotation * other._rotation;
	_scale *= other._scale;
	_Update();

	return *this;
}
