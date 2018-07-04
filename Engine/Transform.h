#pragma once
#include "Vector.h"
#include "Matrix.h"

class Transform
{
private:
	Vector3 _position;

	//Pitch, yaw, roll
	Vector3 _rotation;

	Vector3 _scale;

public:
	Transform() : _scale(1.f, 1.f, 1.f) {}
	~Transform() {}

	inline Vector3 GetPosition() const	{ return _position; }
	inline Vector3 GetRotation() const	{ return _rotation; }
	inline Vector3 GetScale() const		{ return _scale; }

	inline void SetPosition(const Vector3 &position)	{ _position = position; }
	inline void SetRotation(const Vector3 &rotation)	{ _rotation = rotation; }
	inline void SetScale(const Vector3 &scale)			{ _scale = scale;; }

	inline void Move(const Vector3 &delta)				{ _position += delta; }
	inline void Rotate(const Vector3 &delta)			{ _rotation += delta; }

	Mat4 MakeTransformationMatrix() const;
	Mat4 MakeInverseTransformationMatrix() const;

	Vector3 GetForwardVector() const;
	Vector3 GetRightVector() const;
	Vector3 GetUpVector() const;
};
