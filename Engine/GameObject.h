#pragma once
#include "BufferIterator.h"
#include "Map.h"
#include "Transform.h"

class ObjectProperties;

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
	virtual void Render() const {}

	virtual void GetProperties(ObjectProperties &properties) { _AddBaseProperties(properties); }

	inline GameObject* GetParent() const		{ return _parent; }

	inline void SetParent(GameObject &parent)	{ _parent = &parent; }

	Mat4 MakeTransformationMatrix() const;
	Mat4 MakeInverseTransformationMatrix() const;
	void ApplyTransformToShader() const;

	virtual void SaveToFile(BufferIterator<byte>&, const Map<String, uint16> &strings) const {}
	virtual void LoadFromFile(BufferIterator<byte>&, const Map<uint16, String> &strings) {}
};
