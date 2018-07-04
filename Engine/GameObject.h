#pragma once
#include "Transform.h"

class GameObject
{
protected:
	GameObject *_parent;

public:
	Transform transform;

	GameObject() {}
	virtual ~GameObject() {}

	virtual void Update() {}
	virtual void Render() {}

	inline GameObject* GetParent() const		{ return _parent; }

	inline void SetParent(GameObject &parent)	{ _parent = &parent; }

	Mat4 MakeTransformationMatrix() const;
	Mat4 MakeInverseTransformationMatrix() const;
	void ApplyTransformToShader() const;
};
