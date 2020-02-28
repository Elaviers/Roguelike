#pragma once
#include "BufferIterator.hpp"
#include "FunctionPointer.hpp"
#include "Vector.hpp"
#include "Matrix.hpp"
#include "Rotation.hpp"

class Transform
{
	Vector3 _position;
	Rotation _rotation;
	Vector3 _scale;

	Callback _onChanged;
	void _Update()
	{
		_matrixStatus = EMatrixStatus::EMPTY;
		_onChanged.TryCall();
	}

	Mat4 _matrix;
	enum class EMatrixStatus
	{
		EMPTY,
		TRANSFORM,
		INVERSETRANSFORM
	} _matrixStatus;

	void _MakeTransformationMatrix(Mat4&) const;
	void _MakeInverseTransformationMatrix(Mat4&) const;

public:
	Transform(const Vector3& position, const Rotation& rotation = Rotation(), const Vector3& scale = Vector3(1, 1, 1), const Callback& callback = Callback(nullptr)) :
		_position(position), 
		_rotation(rotation), 
		_scale(scale), 
		_onChanged(callback), 
		_matrixStatus(EMatrixStatus::EMPTY) 
	{
		_onChanged.TryCall();
	}

	Transform(const Callback& callback = Callback(nullptr)) : Transform(Vector3(), Rotation(), Vector3(1, 1, 1), callback) {}
	
	Transform(const Transform& other) : 
		_onChanged(),
		_position(other._position), 
		_rotation(other._rotation), 
		_scale(other._scale), 
		_matrix(other._matrix),
		_matrixStatus(other._matrixStatus)
	{}

	Transform(const Transform&& other) noexcept :
		_onChanged(),
		_position(other._position),
		_rotation(other._rotation),
		_scale(other._scale),
		_matrix(other._matrix),
		_matrixStatus(other._matrixStatus)
	{}

	~Transform() {}

	Transform& operator=(const Transform& other)
	{
		_position = other._position;
		_rotation = other._rotation;
		_scale = other._scale;
		_matrix = other._matrix;
		_matrixStatus = other._matrixStatus;

		_Update();
		return *this;
	}

	const Vector3& GetPosition() const			{ return _position; }
	const Rotation& GetRotation() const			{ return _rotation; }
	const Vector3& GetRotationEuler() const		{ return _rotation.GetEuler(); }
	const Vector3& GetScale() const				{ return _scale; }

	void SetPosition(const Vector3 &position)	{ _position = position; _Update(); }
	void SetRotation(const Rotation &rotation)	{ _rotation = rotation; _Update(); }
	void SetRotationEuler(const Vector3& euler) { SetRotation(Rotation(euler)); }
	void SetScale(const Vector3 &scale)			{ _scale = scale;		_Update(); }
	void SetCallback(const Callback& callback)	{ _onChanged = callback; _onChanged.TryCall(); }

	void Move(const Vector3 &delta)				{ _position += delta;				_Update(); }
	void Rotate(const Rotation &delta)			{ _rotation = delta * _rotation;	_Update(); }

	void AddRotation(const Vector3& euler)		{ _rotation += euler; _Update(); }

	Mat4 GetTransformationMatrix() const;
	Mat4 GetInverseTransformationMatrix() const;
	Mat4 MakeTransformationMatrix() const;
	Mat4 MakeInverseTransformationMatrix() const;

	Vector3 GetForwardVector() const			{ return _rotation.GetQuat().GetForwardVector(); }
	Vector3 GetRightVector() const				{ return _rotation.GetQuat().GetRightVector(); }
	Vector3 GetUpVector() const					{ return _rotation.GetQuat().GetUpVector(); }

	Transform& operator*=(const Transform &other);
	Transform operator*(const Transform &other) const
	{
		Transform t = *this;
		t *= other;
		return t;
	}

	Transform Inverse() const { return Transform(_position * -1.f, _rotation.Inverse(), 1.f / _scale); }

	void WriteToBuffer(BufferWriter<byte> &buffer) const;
	void ReadFromBuffer(BufferReader<byte> &buffer);
};
