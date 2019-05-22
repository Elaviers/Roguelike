#pragma once
#include "BufferIterator.h"
#include "FunctionPointer.h"
#include "Vector.h"
#include "Matrix.h"
#include "Rotation.h"

class Transform
{
	Vector3 _position;
	Rotation _rotation;
	Vector3 _scale;

	Callback _onChanged;
	inline void _Update()
	{
		_matrixStatus = MAT_EMPTY;
		if (_onChanged.IsCallable())
			_onChanged();
	}

	Mat4 _matrix;
	enum
	{
		MAT_EMPTY,
		MAT_TRANSFORM,
		MAT_INVERSETRANSFORM
	} _matrixStatus;

	void _MakeTransformationMatrix(Mat4&) const;
	void _MakeInverseTransformationMatrix(Mat4&) const;

public:
	Transform(const Callback &callback = Callback(nullptr)) : _onChanged(callback), _scale(1.f, 1.f, 1.f), _matrixStatus(MAT_EMPTY) {}
	Transform(const Vector3& position, const Rotation& rotation = Rotation(), const Vector3& scale = Vector3(1, 1, 1), const Callback& callback = Callback(nullptr)) :
		_position(position), _rotation(rotation), _scale(scale), _onChanged(callback), _matrixStatus(MAT_EMPTY) {}
	
	~Transform() {}

	inline const Vector3& GetPosition() const			{ return _position; }
	inline const Rotation& GetRotation() const			{ return _rotation; }
	inline const Vector3& GetRotationEuler() const		{ return _rotation.GetEuler(); }
	inline const Vector3& GetScale() const				{ return _scale; }

	inline void SetPosition(const Vector3 &position)	{ _position = position; _Update(); }
	inline void SetRotation(const Rotation &rotation)	{ _rotation = rotation; _Update(); }
	inline void SetRotationEuler(const Vector3& euler)  { SetRotation(Rotation(euler)); }
	inline void SetScale(const Vector3 &scale)			{ _scale = scale;		_Update(); }
	inline void SetCallback(const Callback &callback)	{ _onChanged = callback; }

	inline void Move(const Vector3 &delta)				{ _position += delta;	_Update(); }
	inline void Rotate(const Rotation &delta)			{ _rotation += delta;	_Update(); }

	inline void AddRotation(const Vector3& euler)		{ _rotation.AddEuler(euler); _Update(); }

	const Mat4& GetTransformationMatrix() const;
	const Mat4& GetInverseTransformationMatrix() const;
	Mat4 MakeTransformationMatrix() const;
	Mat4 MakeInverseTransformationMatrix() const;

	inline Vector3 GetForwardVector() const		{ return _rotation.GetQuat().GetForwardVector(); }
	inline Vector3 GetRightVector() const		{ return _rotation.GetQuat().GetRightVector(); }
	inline Vector3 GetUpVector() const			{ return _rotation.GetQuat().GetUpVector(); }

	Transform& operator*=(const Transform &other);
	inline Transform operator*(const Transform &other) const
	{
		Transform t = *this;
		t *= other;
		return t;
	}

	inline Transform Inverse() const { return Transform(_position * -1.f, _rotation.Inverse(), 1.f / _scale); }

	void WriteToBuffer(BufferIterator<byte> &buffer) const;
	void ReadFromBuffer(BufferIterator<byte> &buffer);
};
