#pragma once
#include "Transform.h"
#include "ObjectProperties.h"

class GameObject
{
protected:
	GameObject *_parent;

	void _AddBaseProperties(ObjectProperties&);

public:
	Transform transform;

	GameObject() {}
	virtual ~GameObject() {}

	virtual void Update() {}
	virtual void Render() {}

	virtual void GetProperties(ObjectProperties &properties) { _AddBaseProperties(properties); }

	inline GameObject* GetParent() const		{ return _parent; }

	inline void SetParent(GameObject &parent)	{ _parent = &parent; }

	Mat4 MakeTransformationMatrix() const;
	Mat4 MakeInverseTransformationMatrix() const;
	void ApplyTransformToShader() const;
};
