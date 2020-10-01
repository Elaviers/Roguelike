#pragma once
#include "EEntityID.hpp"
#include <ELCore/ByteReader.hpp>
#include <ELCore/ByteWriter.hpp>
#include <ELCore/NumberedSet.hpp>
#include <ELCore/Event.hpp>
#include <ELCore/PropertyCollection.hpp>
#include <ELMaths/Bounds.hpp>
#include <ELMaths/Transform.hpp>
#include <ELPhys/Collider.hpp>
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
class RenderQueue;
struct Frustum;
struct Ray;
struct RaycastResult;

class Entity
{
public:
	enum class EFlags : byte
	{
		NONE = 0,
		SAVEABLE = 0x1,
		ALWAYS_DRAW = 0x2
	};


private:
	String _name;

	Transform _transform;

	Transform _worldTransform;
	bool _wtValid;

	Bounds _worldBounds;
	bool _wbValid; //true if world bounds don't need recalc

	void _OnTransformChanged();
	void _OnChildChanged();
protected:
	static void* operator new(size_t size) { return ::operator new(size); }
	void operator delete(void* ptr) { ::operator delete(ptr); }

	const uint32 _uid;

	EFlags _flags;
	const bool _dynamic;

	Entity *_parent;
	Buffer<Entity*> _children;

	void _AddBaseProperties(PropertyCollection&);

	void _InvalidateWorldBounds() { _wbValid = false; }

	static uint32 _TakeNextUID()
	{
		static uint32 counter = 1;
		return counter++;
	}

public:
	Event<> onNameChanged;
	Event<> onChildChanged;
	Event<> onTransformChanged;

	Entity_FUNCS(Entity, EEntityID::Entity)

	Entity(EFlags flags = EFlags::NONE, const String &name = String()) : 
		_name(name), 
		_uid(_TakeNextUID()), 
		_flags(flags), 
		_dynamic(false), 
		_parent(nullptr), 
		_transform(Callback(this, &Entity::_OnTransformChanged)),
		_wtValid(false),
		_wbValid(false)
	{}

	Entity(const Entity &other) : 
		_uid(_TakeNextUID()), 
		_flags(other._flags), 
		_dynamic(false), 
		_parent(other._parent), 
		_transform(other._transform),
		_worldBounds(other._worldBounds),
		_wtValid(other._wtValid),
		_wbValid(other._wbValid)
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

	void Delete(const Context&);
	
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
		if (_wtValid)
			return _worldTransform;

		//todo: CONST					CAST					...
		if (_parent)
			const_cast<Entity*>(this)->_worldTransform = _transform * _parent->GetWorldTransform();
		else
			const_cast<Entity*>(this)->_worldTransform = _transform;

		const_cast<Entity*>(this)->_wtValid = true;
		return _worldTransform;
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

		_worldTransform = t;
		_wtValid = true;
	}

	void SetWorldPosition(const Vector3 &v) 
	{ 
		if (_parent)
			SetRelativePosition((Vector4(v, 1.f) * _parent->GetInverseTransformationMatrix()).GetXYZ());
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

	Matrix4 GetTransformationMatrix() const;
	Matrix4 GetInverseTransformationMatrix() const;

	#pragma endregion

	#pragma region Hierachy

	Entity* GetParent() const { return _parent; }
	const Buffer<Entity*>& Children() const { return _children; }

	void SetParent(Entity* parent);

	Entity* FindChild(const String& name);
	Entity* FindChild(uint32 uid);

	const Entity* FindChild(const String& name) const { return const_cast<Entity*>(this)->FindChild(name); }
	const Entity* FindChild(uint32 uid) const { return const_cast<Entity*>(this)->FindChild(uid); }

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

	void DeleteChildren(const Context& ctx)
	{
		while (_children.GetSize())
			_children[0]->Delete(ctx);
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

	EFlags GetFlags() const { return _flags; }
	void SetFlags(EFlags flags) { _flags = flags; }

	virtual void Update(float deltaTime) {}
	virtual void Render(RenderQueue&) const {}

	void UpdateAll(float deltaTime);
	void RenderAll(RenderQueue&, const Frustum& cameraFrustum) const;

	virtual const PropertyCollection& GetProperties();

	virtual Bounds GetBounds() const { return Bounds(); }

	//IO
	void WriteAllToFile(ByteWriter&, NumberedSet<String>& strings, const Context& ctx) const;
	static Entity* CreateFromData(ByteReader&, const NumberedSet<String>& strings, const Context& ctx);

	virtual void WriteData(ByteWriter&, NumberedSet<String>& strings, const Context& ctx) const;
	virtual void ReadData(ByteReader&, const NumberedSet<String>& strings, const Context& ctx);

	//Collision
	virtual const Collider* GetCollider() const { return nullptr; }

	//Sweeps this object along the movement vector
	EOverlapResult OverlapsCollider(const Collider& other, const Transform& otherTransform, const Vector3& sweep = Vector3(), Vector3* out_Penetration = nullptr) const;
	EOverlapResult Overlaps(const Entity& other, const Vector3& sweep = Vector3(), Vector3* out_Penetration = nullptr) const
	{
		const Collider* collider = other.GetCollider();
		if (collider) return OverlapsCollider(*collider, other.GetWorldTransform(), sweep, out_Penetration);
		return EOverlapResult::SEPERATE;
	}

	EOverlapResult AnyPartOverlapsCollider(const Collider& other, const Transform& otherTransform, const Vector3& sweep = Vector3(), Vector3* out_Penetration = nullptr) const;
	EOverlapResult AnyPartOverlaps(const Entity& other, const Vector3& sweep = Vector3(), Vector3* out_Penetration = nullptr) const
	{
		const Collider* collider = other.GetCollider();
		if (collider) return AnyPartOverlapsCollider(*collider, other.GetWorldTransform(), sweep, out_Penetration);
		return EOverlapResult::SEPERATE;
	}

	float MinimumDistanceToCollider(const Collider& other, const Transform& otherTransform, Vector3& out_PointA, Vector3& out_PointB, const Vector3& sweep = Vector3()) const;
	float MinimumDistanceTo(const Entity& other, Vector3& out_PointA, Vector3& out_PointB, const Vector3& sweep = Vector3()) const
	{
		const Collider* collider = other.GetCollider();
		if (collider) return MinimumDistanceToCollider(*collider, other.GetWorldTransform(), out_PointA, out_PointB, sweep);
		return -1.f;;
	}

	Pair<Vector3> GetShallowContactPointsWithCollider(float shrink, const Collider& other, const Transform& otherTransform, float otherShrink) const;
	Pair<Vector3> GetShallowContactPoints(float shrink, const Entity& other, float otherShrink) const
	{
		const Collider* collider = other.GetCollider();
		if (collider) return GetShallowContactPointsWithCollider(shrink, *collider, other.GetWorldTransform(), otherShrink);
		return Pair<Vector3>();
	}

	bool OverlapsRay(const Ray&, RaycastResult& out_Result) const;

	Buffer<RaycastResult> Raycast(const Ray&);
	Buffer<Entity*> FindOverlappingChildren(const Collider& other, const Transform& otherTransform = Transform());

	//Operators
	Entity& operator=(const Entity&) = delete;
	Entity& operator=(Entity&& other) noexcept;

	bool operator==(const Entity &other) const { return _uid == other._uid; }

	Bounds GetWorldBounds() const;

	template<typename T>
	bool IsType() const { return dynamic_cast<const T*>(this) != nullptr; }

	//Commands
	void CMD_List(const Buffer<String>& args, const Context& ctx);
	void CMD_Ent(const Buffer<String>& args, const Context& ctx);

protected:
	template<typename T>
	static T* _CreateCopy(const T& other)
	{
		Entity *go = new T(other);
		const_cast<bool&>(go->_dynamic) = true;
		const_cast<EFlags&>(go->_flags) = other._flags;
		go->_parent = nullptr;
		go->SetParent(other._parent); //force set parent
		return dynamic_cast<T*>(go);
	}
};

#include <ELCore/MacroUtilities.hpp>
DEFINE_BITMASK_FUNCS(Entity::EFlags, byte)
