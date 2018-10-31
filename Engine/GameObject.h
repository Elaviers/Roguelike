#pragma once
#include "Bounds.h"
#include "BufferIterator.h"
#include "Map.h"
#include "Transform.h"

class Collider;
class ObjectProperties;
struct Ray;
struct RaycastResult;

class GameObject
{
protected:
	GameObject *_parent;
	Buffer<GameObject*> _children;

	void _AddBaseProperties(ObjectProperties&);

	virtual void _OnTransformChanged() {}

public:
	Transform transform;
	//Note: transform is relative to parent!

	GameObject() : _parent(nullptr), transform(Callback(this, &GameObject::_OnTransformChanged)) {}
	virtual ~GameObject()
	{ 
		if (_parent) 
			_parent->_children.Remove(this);
	}

	//Hierachy
	inline GameObject* GetParent() const { return _parent; }
	inline void SetParent(GameObject *parent)
	{ 
		if (_parent)
			_parent->_children.Remove(this);

		_parent = parent;

		if (_parent)
			_parent->_children.Add(this); 
	}

	//Transform
	Mat4 GetTransformationMatrix();
	Mat4 MakeInverseTransformationMatrix() const;
	void ApplyTransformToShader() const;

	//General
	virtual void Update() {}
	virtual void Render() const {}

	virtual void GetProperties(ObjectProperties &properties) { _AddBaseProperties(properties); }

	//File IO
	virtual void SaveToFile(BufferIterator<byte>&, const Map<String, uint16> &strings) const {}
	virtual void LoadFromFile(BufferIterator<byte>&, const Map<uint16, String> &strings) {}

	//Collision
	virtual const Collider* GetCollider() const { return nullptr; }
	virtual bool Raycast(const Ray&, RaycastResult&) const;
	virtual bool Overlaps(const Collider&, const Transform&) const;

	//Other
	inline GameObject& operator=(const GameObject &other)
	{
		SetParent(other._parent);

		_children.SetSize(0);

		transform = other.transform;
		transform.SetCallback(Callback(this, &GameObject::_OnTransformChanged));

		return *this;
	}

	virtual Bounds GetBounds() const { return Bounds(); }
};
