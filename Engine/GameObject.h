#pragma once
#include "Bounds.h"
#include "BufferIterator.h"
#include "Map.h"
#include "NumberedSet.h"
#include "Transform.h"
#include <cstdlib>

#define GAMEOBJECT_FUNCS(CLASSNAME)															\
public:																						\
	virtual size_t SizeOf() const { return sizeof(*this); }									\
	virtual GameObject* Clone() const { return GameObject::_CreateCopy<CLASSNAME>(*this); } \
	static CLASSNAME* Create() { CLASSNAME* obj = new CLASSNAME(); const_cast<bool&>(obj->_dynamic) = true; return obj; }

class Collider;
class ObjCamera; //!!! todo - make this forward declaration unnecessary
class CvarMap;
struct Ray;
struct RaycastResult;

class GameObject
{
protected:
	static void* operator new(size_t size) { return ::operator new(size); }
	
	const uint32 _uid;

	const byte _flags;
	const bool _dynamic;

	GameObject *_parent;
	Buffer<GameObject*> _children;

	void _AddBaseCvars(CvarMap&);

	virtual void _OnTransformChanged() {}

public:
	enum
	{
		FLAG_SAVEABLE = 0x1,
		FLAG_DBG_ALWAYS_DRAW = 0x2
	};

	GAMEOBJECT_FUNCS(GameObject)

	Transform transform;
	//Note: transform is relative to parent!

	GameObject(byte flags = 0) : _uid(GetNextUID()), _flags(flags), _dynamic(false), _parent(nullptr), transform(Callback(this, &GameObject::_OnTransformChanged)) {}

	GameObject(const GameObject &other) : _uid(GetNextUID()), _flags(other._flags), _dynamic(false), _parent(other._parent), transform(other.transform)
	{
		transform.SetCallback(Callback(this, &GameObject::_OnTransformChanged));
		CloneChildrenFrom(other);
	}

	GameObject(GameObject &&other) : _uid(other._uid), _flags(other._flags), _dynamic(false) 
	{ 
		operator=(std::move(other)); 
		
		if (other._dynamic)
			delete &other;
	}

	virtual ~GameObject()
	{
		DeleteChildren();

		if (_parent)
			_parent->_children.Remove(this);
	}
	

	//Hierachy
	inline GameObject* GetParent() const { return _parent; }
	inline const Buffer<GameObject*>& Children() const { return _children; }

	inline void SetParent(GameObject *parent)
	{ 
		if (_parent)
			_parent->_children.Remove(this);

		_parent = parent;

		if (_parent)
			_parent->_children.Add(this); 
	}

	inline void DeleteChildren()
	{
		while (_children.GetSize() > 0)
			if (_children[0]->_dynamic)
				delete _children[0];
			else
				_children[0]->SetParent(nullptr);
	}

	inline void CloneChildrenFrom(const GameObject &src)
	{
		for (uint32 i = 0; i < src._children.GetSize(); ++i)
			src._children[i]->Clone()->SetParent(this);
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
	inline byte GetFlags() const { return _flags; }

	virtual void Update() {}
	virtual void Render() const {}
	void Render(const ObjCamera &camera) const;

	virtual void GetCvars(CvarMap &properties) { _AddBaseCvars(properties); }

	//File IO
	void WriteAllToFile(BufferIterator<byte>&, NumberedSet<String> &strings) const;
	virtual void WriteToFile(BufferIterator<byte>&, NumberedSet<String> &strings) const {}
	virtual void ReadFromFile(BufferIterator<byte>&, const NumberedSet<String> &strings) {}

	//Collision
	virtual const Collider* GetCollider() const { return nullptr; }

	virtual bool OverlapsCollider(const Collider&, const Transform&) const;
	virtual bool OverlapsRay(const Ray&, RaycastResult&) const;

	Buffer<RaycastResult> Raycast(const Ray&);
	Buffer<GameObject*> FindOverlaps(const Collider&, const Transform& = Transform());

	//Operators
	GameObject& operator=(const GameObject&) = delete;

	inline GameObject& operator=(GameObject &&other)
	{
		SetParent(other._parent);
		other.SetParent(nullptr);

		for (uint32 i = 0; i < other._children.GetSize(); ++i)
			other._children[i]->SetParent(this);

		transform = other.transform;
		transform.SetCallback(Callback(this, &GameObject::_OnTransformChanged));

		return *this;
	}

	inline bool operator==(const GameObject &other) const { return _uid == other._uid; }

	//Other
	virtual Bounds GetBounds() const { return Bounds(); }
	inline Bounds GetWorldBounds() const 
	{
		Bounds b = GetBounds();
		const Mat4& wt = GetWorldTransform().GetTransformationMatrix();
		return Bounds(b.min * wt, b.max * wt);
	}

	template<typename T>
	inline bool IsType() { return dynamic_cast<T*>(this) != nullptr; }

	template <typename T>
	inline T* NewChild()
	{
		GameObject* object = new T();
		object->_dynamic = true;
		object->SetParent(this);
		return dynamic_cast<T*>(object);
	}

	//Static
	static uint32 GetNextUID() 
	{
		static uint32 counter = 1;
		return counter++;
	}

protected:
	template<typename T>
	static T* _CreateCopy(const T& other)
	{
		GameObject *go = new T(other);
		const_cast<bool&>(go->_dynamic) = true;
		return dynamic_cast<T*>(go);
	}
};
