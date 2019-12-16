#pragma once
#include "Bounds.hpp"
#include "BufferIterator.hpp"
#include "Event.hpp"
#include "Map.hpp"
#include "NumberedSet.hpp"
#include "EntityID.hpp"
#include "RenderChannels.hpp"
#include "PropertyCollection.hpp"
#include "Transform.hpp"
#include <cstdlib>

#define Entity_FUNCS(CLASSNAME, ID)																							\
public:																														\
	virtual byte GetTypeID() const { return (byte)(ID); }																	\
	virtual const char* GetClassString() const { return #CLASSNAME;}														\
	virtual size_t SizeOf() const { return sizeof(*this); }																	\
	virtual Entity* Clone() const { return Entity::_CreateCopy<CLASSNAME>(*this); }											\
	CLASSNAME* TypedClone() const { return (CLASSNAME*)Clone(); }															\
																															\
	static CLASSNAME* Create() { CLASSNAME* obj = new CLASSNAME(); const_cast<bool&>(obj->_dynamic) = true; return obj; }	\
	static const byte TypeID = (byte)(ID);

class Collider;
class EntCamera; //!!! todo - make this forward declaration unnecessary
struct Ray;
struct RaycastResult;

class Entity
{
public:
	enum class Flags : byte
	{
		NONE = 0,
		SAVEABLE = 0x1,
		DBG_ALWAYS_DRAW = 0x2
	};


private:
	Transform _transform;

	String _name;

	void _OnTransformChanged() { onTransformChanged(); }

	void _OnChildChanged()
	{
		onChildChanged();

		if (_parent)
			_parent->_OnChildChanged();
	}
protected:
	static void* operator new(size_t size) { return ::operator new(size); }
	void operator delete(void* ptr) { ::operator delete(ptr); }

	const uint32 _uid;

	Flags _flags;
	const bool _dynamic;

	Entity *_parent;
	Buffer<Entity*> _children;

	void _AddBaseProperties(PropertyCollection&);

	static uint32 _TakeNextUID()
	{
		static uint32 counter = 1;
		return counter++;
	}

public:
	Event<> onNameChanged;
	Event<> onChildChanged;
	Event<> onTransformChanged;

	Entity_FUNCS(Entity, EntityID::Entity)

	Entity(Flags flags = Flags::NONE, const String &name = String()) : 
		_name(name), 
		_uid(_TakeNextUID()), 
		_flags(flags), 
		_dynamic(false), 
		_parent(nullptr), 
		_transform(Callback(this, &Entity::_OnTransformChanged)) 
	{}

	Entity(const Entity &other) : 
		_uid(_TakeNextUID()), 
		_flags(other._flags), 
		_dynamic(false), 
		_parent(other._parent), 
		_transform(other._transform)
	{
		_transform.SetCallback(Callback(this, &Entity::_OnTransformChanged));
		CloneChildrenFrom(other);
	}

	Entity(Entity &&other) noexcept : _parent(nullptr), _uid(other._uid), _flags(other._flags), _dynamic(false) 
	{ 
		operator=(std::move(other)); 
		
		if (other._dynamic)
			delete &other;
	}

	virtual ~Entity() {}

	void Delete();
	
	#pragma region Transform

	const Transform& GetRelativeTransform() const			{ return _transform; }
	const Vector3& GetRelativePosition() const				{ return GetRelativeTransform().GetPosition(); }
	const Rotation& GetRelativeRotation() const				{ return GetRelativeTransform().GetRotation(); }
	const Vector3& GetRelativeScale() const					{ return GetRelativeTransform().GetScale(); }

	void AddRelativeRotation(const Vector3& euler)			{ _transform.AddRotation(euler); }

	void SetRelativeTransform(const Transform& transform)	{ _transform = transform; }
	void SetRelativePosition(const Vector3 &v)				{ _transform.SetPosition(v); }
	void SetRelativeRotation(const Rotation &r)				{ _transform.SetRotation(r); }
	void SetRelativeScale(const Vector3 &v)					{ _transform.SetScale(v); }

	void RelativeMove(const Vector3& v)						{ _transform.Move(v); }
	void RelativeRotate(const Rotation& q)					{ _transform.Rotate(q); }

	Transform GetWorldTransform() const
	{
		if (_parent)
			return _transform * _parent->GetWorldTransform();

		return _transform;
	}

	Vector3 GetWorldPosition() const						{ return GetWorldTransform().GetPosition(); }
	Rotation GetWorldRotation() const						{ return GetWorldTransform().GetRotation(); }
	Vector3 GetWorldScale() const							{ return GetWorldTransform().GetScale(); }

	void SetWorldTransform(const Transform &t)
	{
		if (_parent)
			SetRelativeTransform(t * _parent->GetWorldTransform().Inverse());
		else
			SetRelativeTransform(t);
	}

	void SetWorldPosition(const Vector3 &v) 
	{ 
		if (_parent)
			SetRelativePosition(v * _parent->GetInverseTransformationMatrix());
		else
			SetRelativePosition(v);
	}

	void SetWorldRotation(const Rotation &rotation) {
		if (_parent)
			SetRelativeRotation(rotation * _parent->GetWorldTransform().GetRotation().Inverse());
		else
			SetRelativeRotation(rotation);
	}

	void SetWorldScale(const Vector3& v)
	{
		if (_parent)
			SetRelativeScale(v / _parent->GetWorldTransform().GetScale());
		else
			SetRelativeScale(v);
	}

	Mat4 GetTransformationMatrix() const;
	Mat4 GetInverseTransformationMatrix() const;

	#pragma endregion

	#pragma region Hierachy

	Entity* GetParent() const { return _parent; }
	const Buffer<Entity*>& Children() const { return _children; }

	void SetParent(Entity* parent);

	Entity* FindChildWithName(const String& name);
	Entity* FindChildWithUID(uint32 uid);

	template<typename T>
	void FindChildrenOfType(Buffer<T*>& out)
	{
		if (_children.GetSize())
		{
			size_t firstIndex = out.GetSize();
			out.Append(_children.GetSize());

			for (size_t i = 0; i < _children.GetSize(); ++i)
				if (_children[i]->IsType<T>())
					out[firstIndex + i] = (T*)_children[i];

			for (size_t i = 0; i < _children.GetSize(); ++i)
				_children[i]->FindChildrenOfType<T>(out);
		}
	}

	template<typename T>
	Buffer<T*> FindChildrenOfType() 
	{ 
		Buffer<T*> buffer; 
		FindChildrenOfType<T>(buffer); 
		return buffer; 
	}

	void CloneChildrenFrom(const Entity &src)
	{
		for (size_t i = 0; i < src._children.GetSize(); ++i)
			src._children[i]->Clone()->SetParent(this);
	}

	void DeleteChildren()
	{
		while (_children.GetSize())
			_children[0]->Delete();
	}

	bool IsChildOf(const Entity* parent) const
	{
		if (_parent == nullptr)
			return false;

		if (_parent == parent)
			return true;

		return _parent->IsChildOf(parent);
	}

	#pragma endregion

	//General
	uint32 GetUID() const { return _uid; }
	
	const String& GetName() const { return _name; }
	void SetName(const String& name) { _name = name; onNameChanged(); if (_parent) _parent->_OnChildChanged(); }

	Flags GetFlags() const { return _flags; }
	void SetFlags(Flags flags) { _flags = flags; }

	virtual void Update(float deltaTime) {}
	virtual void Render(RenderChannels) const {}

	void UpdateAll(float deltaTime);
	void RenderAll(const EntCamera &camera, RenderChannels) const;

	virtual const PropertyCollection& GetProperties();

	//IO
	void WriteAllToFile(BufferWriter<byte>&, NumberedSet<String> &strings) const;
	static Entity* CreateFromData(BufferReader<byte>&, const NumberedSet<String>& strings);

	virtual void WriteData(BufferWriter<byte>&, NumberedSet<String>& strings) const;
	virtual void ReadData(BufferReader<byte>&, const NumberedSet<String>& strings);

	//Collision
	virtual const Collider* GetCollider() const { return nullptr; }

	virtual bool OverlapsCollider(const Collider&, const Transform&) const;
	virtual bool OverlapsRay(const Ray&, RaycastResult&) const;

	Buffer<RaycastResult> Raycast(const Ray&);
	Buffer<Entity*> FindOverlaps(const Collider&, const Transform& = Transform());

	//Operators
	Entity& operator=(const Entity&) = delete;

	Entity& operator=(Entity&& other) noexcept;

	bool operator==(const Entity &other) const { return _uid == other._uid; }

	//Other
	virtual Bounds GetBounds() const { return Bounds(); }

	Bounds GetWorldBounds(bool noTranslation = false) const;

	template<typename T>
	bool IsType() { return dynamic_cast<T*>(this) != nullptr; }

	//Commands
	void CMD_List(const Buffer<String>& args);
	void CMD_Ent(const Buffer<String>& args);

protected:
	template<typename T>
	static T* _CreateCopy(const T& other)
	{
		Entity *go = new T(other);
		const_cast<bool&>(go->_dynamic) = true;
		go->SetParent(other._parent);
		const_cast<Flags&>(go->_flags) = other._flags;
		return dynamic_cast<T*>(go);
	}
};

#include "MacroUtilities.hpp"
DEFINE_BITMASK_FUNCS(Entity::Flags)
