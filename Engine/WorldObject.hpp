#pragma once
#include "EObjectID.hpp"
#include "ObjectIOContext.hpp"
#include <ELCore/List.hpp>
#include <ELCore/Text.hpp>
#include <ELMaths/Frustum.hpp>
#include <ELMaths/Transform.hpp>

template <typename T>
class NumberedSet;

class Collider;
class PropertyCollection;
class RenderQueue;
class Volume;
class World;

#define WORLDOBJECT_VFUNCS(CLASSNAME, ID)								\
protected:																\
	virtual WorldObject* _Clone() const { return new CLASSNAME(*this); } \
public:																	\
	friend World;														\
	static const EObjectID TypeID = ID;									\
	virtual EObjectID GetTypeID() const { return TypeID; }				\
	virtual const char* GetClassString() const { return #CLASSNAME; }	\
	virtual size_t SizeOf() const { return sizeof(*this); }

class WorldObject
{
	World* _world;
	uint32 _uid;
	bool _pendingDestruction;

	WorldObject* _parent;
	List<WorldObject*> _children;

	Text _name;
	Transform _relativeTransform;
	Transform _absoluteTransform;

private:
	void _Relative2Absolute();
	void _Absolute2Relative();

protected:
	WorldObject(World& world);
	WorldObject(const WorldObject&);
	WorldObject(WorldObject&&) = delete;

	virtual void _OnCreated();
	virtual void _OnCloned();
	virtual void _OnTransformChanged();

	static const PropertyCollection& _GetNonTransformProperties();

public:
	WORLDOBJECT_VFUNCS(WorldObject, EObjectID::OBJECT);

	virtual ~WorldObject();

	//Member access
	World& GetWorld() const { return *_world; }
	uint32 GetUID() const { return _uid; }
	bool IsPendingDestruction() const { return _pendingDestruction; }

	const WorldObject* GetParent() const { return _parent; } //Changing the parent could change this so a const pointer is returned here
	WorldObject* GetParent() { return _parent; }
	const List<const WorldObject*>& GetChildren() const { return reinterpret_cast<const List<const WorldObject*>&>(_children); }
	const List<WorldObject*>& GetChildren() { return _children; }

	const Text& GetName() const { return _name; }
	const Transform& GetRelativeTransform() const { return _relativeTransform; }
	const Vector3& GetRelativePosition() const { return _relativeTransform.GetPosition(); }
	const Rotation& GetRelativeRotation() const { return _relativeTransform.GetRotation(); }
	const Vector3& GetRelativeScale() const { return _relativeTransform.GetScale(); }
	const Transform& GetAbsoluteTransform() const { return _absoluteTransform; }
	const Vector3& GetAbsolutePosition() const { return _absoluteTransform.GetPosition(); }
	const Rotation& GetAbsoluteRotation() const { return _absoluteTransform.GetRotation(); }
	const Vector3& GetAbsoluteScale() const { return _absoluteTransform.GetScale(); }
	Vector3 GetRightVector() const { return _absoluteTransform.GetRightVector(); }
	Vector3 GetUpVector() const { return _absoluteTransform.GetUpVector(); }
	Vector3 GetForwardVector() const { return _absoluteTransform.GetForwardVector(); }

	void Destroy();

	void SetParent(WorldObject* parent, bool keepWorldTransform);

	void SetName(const Text& name);
	void SetRelativeTransform(const Transform& relativeTransform) { _relativeTransform = relativeTransform; _Relative2Absolute(); }
	void SetRelativePosition(const Vector3& relativePosition) { _relativeTransform.SetPosition(relativePosition); _Relative2Absolute(); }
	void SetRelativeRotation(const Rotation& relativeRotation) { _relativeTransform.SetRotation(relativeRotation); _Relative2Absolute(); }
	void SetRelativeScale(const Vector3& relativeScale) { _relativeTransform.SetScale(relativeScale); _Relative2Absolute(); }
	void SetAbsoluteTransform(const Transform& absoluteTransform) { _absoluteTransform = absoluteTransform; _Absolute2Relative(); }
	void SetAbsolutePosition(const Vector3& absolutePosition) { _absoluteTransform.SetPosition(absolutePosition); _Absolute2Relative(); }
	void SetAbsoluteRotation(const Rotation& absoluteRotation) { _absoluteTransform.SetRotation(absoluteRotation); _Absolute2Relative(); }
	void SetAbsoluteScale(const Vector3& absoluteScale) { _absoluteTransform.SetScale(absoluteScale); _Absolute2Relative(); }

	void MoveRelative(const Vector3& offset) { _relativeTransform.Move(offset); _Relative2Absolute(); }
	void MoveAbsolute(const Vector3& offset) { _absoluteTransform.Move(offset); _Absolute2Relative(); }
	void MoveAligned(const Vector3& offset);
	void RotateRelative(const Rotation& rotation) { _relativeTransform.Rotate(rotation); _Relative2Absolute(); }
	void AddRelativeRotation(const Vector3& euler) { _relativeTransform.AddRotation(euler); _Relative2Absolute(); }
	void RotateAbsolute(const Rotation& rotation) { _absoluteTransform.Rotate(rotation); _Absolute2Relative(); }
	void AddAbsoluteRotation(const Vector3& euler) { _absoluteTransform.AddRotation(euler); _Absolute2Relative(); }
	
	//virtual methods
	virtual void Update(float deltaSeconds) {}
	virtual void Render(RenderQueue&) const {}

	virtual const Volume* GetVolume() const { return nullptr; }
	virtual bool IsVisible(const Frustum& view) const;

	virtual void Read(ByteReader& reader, ObjectIOContext& ctx);
	virtual void Write(ByteWriter& writer, ObjectIOContext& ctx) const;
	
	//overrides may use the green channel how they want
	virtual void RenderID(RenderQueue&, uint32 red) const;

	//Utility
	bool IsChildOf(const WorldObject& object) const;

	//operators
	bool operator==(const WorldObject& other) const { return _uid == other._uid; }
	bool operator!=(const WorldObject& other) const { return _uid != other._uid; }

	WorldObject& operator=(const WorldObject&) = delete;
	WorldObject& operator=(WorldObject&&) = delete;

	//Property stuff
	virtual const PropertyCollection& GetProperties();

private:
	uint32 _P_GetParentUID() const;
	void _P_SetParentByUID(const uint32& uid);

	const String& _P_GetName() const;
	void _P_SetName(const String& name);

	const Vector3& _P_GetRotationEuler() const;
	void _P_SetRotationEuler(const Vector3& euler);
};
