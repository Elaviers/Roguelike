#pragma once
#include "Bounds.h"
#include "BufferIterator.h"
#include "Map.h"
#include "Transform.h"

#define GAMEOBJ_STD_OVERRIDES virtual size_t SizeOf() const override { return sizeof(this); }

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
		while (_children.GetSize() > 0)
			_children[0]->SetParent(nullptr);

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
	Mat4 GetTransformationMatrix() const;
	Mat4 GetInverseTransformationMatrix() const;


	inline Transform GetWorldTransform() const
	{
		if (_parent)
			return transform * _parent->transform;

		return transform;
	}

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
	inline Bounds GetWorldBounds() const 
	{
		Bounds b = GetBounds();
		const Mat4& wt = GetWorldTransform().GetTransformationMatrix();
		return Bounds(b.min * wt, b.max * wt);
	}

	virtual size_t SizeOf() const { return sizeof(this); }
};
