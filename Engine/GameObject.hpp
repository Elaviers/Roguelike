#pragma once
#include "Bounds.hpp"
#include "BufferIterator.hpp"
#include "Event.hpp"
#include "Map.hpp"
#include "NumberedSet.hpp"
#include "ObjectIDS.hpp"
#include "RenderChannel.hpp"
#include "PropertyCollection.hpp"
#include "Transform.hpp"
#include <cstdlib>

#define GAMEOBJECT_FUNCS(CLASSNAME, ID)														\
public:																						\
	virtual ObjectIDS::ObjectID GetTypeID() const { return ID; }							\
	virtual size_t SizeOf() const { return sizeof(*this); }									\
	virtual GameObject* Clone() const { return GameObject::_CreateCopy<CLASSNAME>(*this); } \
	inline CLASSNAME* TypedClone() const { return (CLASSNAME*)Clone(); }					\
	static CLASSNAME* Create() { CLASSNAME* obj = new CLASSNAME(); const_cast<bool&>(obj->_dynamic) = true; return obj; }

class Collider;
class ObjCamera; //!!! todo - make this forward declaration unnecessary
struct Ray;
struct RaycastResult;

class GameObject
{
	Transform _transform;

	String _name;

	void _OnTransformChanged() { onTransformChanged(); }

	inline void _OnChildChanged()
	{
		onChildChanged();

		if (_parent)
			_parent->_OnChildChanged();
	}
protected:
	static void* operator new(size_t size) { return ::operator new(size); }
	void operator delete(void* ptr) { ::operator delete(ptr); }

	const uint32 _uid;

	byte _flags;
	const bool _dynamic;

	GameObject *_parent;
	Buffer<GameObject*> _children;

	void _AddBaseProperties(PropertyCollection&);

public:
	Event<> onNameChanged;
	Event<> onChildChanged;
	Event<> onTransformChanged;

	enum Flag
	{
		FLAG_SAVEABLE = 0x1,
		FLAG_DBG_ALWAYS_DRAW = 0x2
	};

	GAMEOBJECT_FUNCS(GameObject, ObjectIDS::GAMEOBJECT)

	GameObject(byte flags = 0, const String &name = String()) : 
		_name(name), 
		_uid(GetNextUID()), 
		_flags(flags), 
		_dynamic(false), 
		_parent(nullptr), 
		_transform(Callback(this, &GameObject::_OnTransformChanged)) 
	{}

	GameObject(const GameObject &other) : 
		_uid(GetNextUID()), 
		_flags(other._flags), 
		_dynamic(false), 
		_parent(other._parent), 
		_transform(other._transform)
	{
		_transform.SetCallback(Callback(this, &GameObject::_OnTransformChanged));
		CloneChildrenFrom(other);
	}

	GameObject(GameObject &&other) noexcept : _parent(nullptr), _uid(other._uid), _flags(other._flags), _dynamic(false) 
	{ 
		operator=(std::move(other)); 
		
		if (other._dynamic)
			delete &other;
	}

	virtual ~GameObject() {}

	void Delete();

	inline void DeleteChildren()
	{
		while (_children.GetSize())
			_children[0]->Delete();
	}
	
	//Transform
	inline const Transform& GetRelativeTransform() const			{ return _transform; }
	inline const Vector3& GetRelativePosition() const				{ return GetRelativeTransform().GetPosition(); }
	inline const Rotation& GetRelativeRotation() const				{ return GetRelativeTransform().GetRotation(); }
	inline const Vector3& GetRelativeScale() const					{ return GetRelativeTransform().GetScale(); }

	inline void AddRelativeRotation(const Vector3& euler)			{ _transform.AddRotation(euler); }

	inline void SetRelativeTransform(const Transform& transform)	{ _transform = transform; }
	inline void SetRelativePosition(const Vector3 &v)				{ _transform.SetPosition(v); }
	inline void SetRelativeRotation(const Rotation &r)				{ _transform.SetRotation(r); }
	inline void SetRelativeScale(const Vector3 &v)					{ _transform.SetScale(v); }

	inline void RelativeMove(const Vector3& v)						{ _transform.Move(v); }
	inline void RelativeRotate(const Rotation& q)					{ _transform.Rotate(q); }

	inline Transform GetWorldTransform() const
	{
		if (_parent)
			return _transform * _parent->GetWorldTransform();

		return _transform;
	}

	inline Vector3 GetWorldPosition() const							{ return GetWorldTransform().GetPosition(); }
	inline Rotation GetWorldRotation() const						{ return GetWorldTransform().GetRotation(); }
	inline Vector3 GetWorldScale() const							{ return GetWorldTransform().GetScale(); }

	inline void SetWorldTransform(const Transform &t)
	{
		if (_parent)
			SetRelativeTransform(t * _parent->GetWorldTransform().Inverse());
		else
			SetRelativeTransform(t);
	}

	inline void SetWorldPosition(const Vector3 &v) 
	{ 
		if (_parent)
			SetRelativePosition(v * _parent->GetInverseTransformationMatrix());
		else
			SetRelativePosition(v);
	}

	inline void SetWorldRotation(const Rotation &rotation) {
		if (_parent)
			SetRelativeRotation(rotation * _parent->GetWorldTransform().GetRotation().Inverse());
		else
			SetRelativeRotation(rotation);
	}

	inline void SetWorldScale(const Vector3& v)
	{
		if (_parent)
			SetRelativeScale(v / _parent->GetWorldTransform().GetScale());
		else
			SetRelativeScale(v);
	}

	Mat4 GetTransformationMatrix() const;
	Mat4 GetInverseTransformationMatrix() const;

	//Hierachy
	inline GameObject* GetParent() const { return _parent; }
	inline const Buffer<GameObject*>& Children() const { return _children; }

	GameObject* FindByUID(uint32 uid)
	{
		if (_uid == uid)
			return this;

		for (size_t i = 0; i < _children.GetSize(); ++i)
		{
			GameObject* result = _children[i]->FindByUID(uid);
			if (result) return result;
		}

		return nullptr;
	}

	template<typename T>
	void FindChildrenOfType(Buffer<T*> &out, bool searchChildren)
	{
		for (size_t i = 0; i < _children.GetSize(); ++i)
		{
			if (_children[i]->IsType<T>())
				out.Add((T*)_children[i]);

			if (searchChildren)
				_children[i]->FindChildrenOfType<T>(out, true);
		}
	}

	template<typename T>
	inline Buffer<T*> FindChildrenOfType(bool searchChildren) { Buffer<T*> buffer; FindChildrenOfType<T>(buffer, searchChildren); return buffer; }

	void SetParent(GameObject *parent)
	{ 
		if (_parent)
		{
			_parent->_children.Remove(this);
			_parent->_OnChildChanged();
		}

		_parent = parent;

		if (_parent)
		{
			_parent->_children.Add(this);
			_parent->_OnChildChanged();
		}
	}

	inline void CloneChildrenFrom(const GameObject &src)
	{
		for (uint32 i = 0; i < src._children.GetSize(); ++i)
			src._children[i]->Clone()->SetParent(this);
	}

	inline bool IsChildOf(const GameObject* parent) const
	{
		if (_parent == nullptr)
			return false;

		if (_parent == parent)
			return true;

		return _parent->IsChildOf(parent);
	}

	//General
	inline const String& GetName() const { return _name; }
	inline void SetName(const String& name) { _name = name; onNameChanged(); if (_parent) _parent->_OnChildChanged(); }

	inline byte GetFlags() const { return _flags; }
	inline void SetFlags(byte flags) { _flags = flags; }

	virtual void Update() {}
	virtual void Render(EnumRenderChannel channels) const {}
	void Render(const ObjCamera &camera, EnumRenderChannel channels) const;

	virtual const PropertyCollection& GetProperties();

	inline uint32 GetUID() const { return _uid; }

	//File IO
	void WriteAllToFile(BufferWriter<byte>&, NumberedSet<String> &strings) const;
	static GameObject* CreateFromData(BufferReader<byte>&, const NumberedSet<String>& strings);

	virtual void WriteData(BufferWriter<byte>&, NumberedSet<String>& strings) const;
	virtual void ReadData(BufferReader<byte>&, const NumberedSet<String>& strings);

	//Collision
	virtual const Collider* GetCollider() const { return nullptr; }

	virtual bool OverlapsCollider(const Collider&, const Transform&) const;
	virtual bool OverlapsRay(const Ray&, RaycastResult&) const;

	Buffer<RaycastResult> Raycast(const Ray&);
	Buffer<GameObject*> FindOverlaps(const Collider&, const Transform& = Transform());

	//Operators
	GameObject& operator=(const GameObject&) = delete;

	inline GameObject& operator=(GameObject &&other) noexcept
	{
		SetParent(other._parent);
		other.SetParent(nullptr);

		for (uint32 i = 0; i < other._children.GetSize(); ++i)
			other._children[i]->SetParent(this);

		_transform = other._transform;
		_transform.SetCallback(Callback(this, &GameObject::_OnTransformChanged));

		return *this;
	}

	inline bool operator==(const GameObject &other) const { return _uid == other._uid; }

	//Other
	virtual Bounds GetBounds() const { return Bounds(); }

	inline Bounds GetWorldBounds(bool noTranslation = false) const 
	{
		Bounds b = GetBounds();
		Mat4 wt = GetWorldTransform().GetTransformationMatrix();
		if (noTranslation) wt[3][0] = wt[3][1] = wt[3][2] = 0.f;

		//Yeah, this is not optimised at all...
		Vector3 testPoints[8] = {
			b.min * wt,
			b.max * wt,
			Vector3(b.min[0], b.min[1], b.max[2]) * wt,
			Vector3(b.min[0], b.max[1], b.min[2]) * wt,
			Vector3(b.min[0], b.max[1], b.max[2]) * wt,
			Vector3(b.max[0], b.min[1], b.min[2]) * wt,
			Vector3(b.max[0], b.min[1], b.max[2]) * wt,
			Vector3(b.max[0], b.max[1], b.min[2]) * wt
		};

		Vector3 min = testPoints[0];
		Vector3 max = min;

		for (int i = 0; i < 8; ++i)
		{
			min = Vector3(Utilities::Min(min[0], testPoints[i][0]), Utilities::Min(min[1], testPoints[i][1]), Utilities::Min(min[2], testPoints[i][2]));
			max = Vector3(Utilities::Max(max[0], testPoints[i][0]), Utilities::Max(max[1], testPoints[i][1]), Utilities::Max(max[2], testPoints[i][2]));
		}

		return Bounds(min, max);
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
		go->SetParent(other._parent);
		const_cast<byte&>(go->_flags) = other._flags;
		return dynamic_cast<T*>(go);
	}
};
