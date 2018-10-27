#pragma once
#include "BufferIterator.h"
#include "Map.h"
#include "Transform.h"

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

	inline GameObject* GetParent() const { return _parent; }
	inline void SetParent(GameObject *parent)
	{ 
		if (_parent)
			_parent->_children.Remove(this);

		_parent = parent;

		if (_parent)
			_parent->_children.Add(this); 
	}

	Mat4 GetTransformationMatrix();
	Mat4 MakeInverseTransformationMatrix() const;
	void ApplyTransformToShader() const;

	inline GameObject& operator=(const GameObject &other)
	{
		SetParent(other._parent);

		_children.SetSize(0);

		transform = other.transform;
		transform.SetCallback(Callback(this, &GameObject::_OnTransformChanged));

		return *this;
	}

	virtual void Update() {}
	virtual void Render() const {}

	virtual void GetProperties(ObjectProperties &properties) { _AddBaseProperties(properties); }

	virtual void SaveToFile(BufferIterator<byte>&, const Map<String, uint16> &strings) const {}
	virtual void LoadFromFile(BufferIterator<byte>&, const Map<uint16, String> &strings) {}

	virtual bool Raycast(const Ray&, RaycastResult&) const { return false; }
};
