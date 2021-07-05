#include "WorldObject.hpp"
#include "World.hpp"
#include <ELCore/PropertyCollection.hpp>
#include <ELCore/MacroUtilities.hpp>
#include <ELGraphics/RenderQueue.hpp>

WorldObject::WorldObject(World& world) :
	_world(&world),
	_uid(world.TakeNextUID()),
	_pendingDestruction(false),
	_parent(nullptr)
{}

WorldObject::WorldObject(const WorldObject& other) : 
	_world(other._world), 
	_uid(other._world->TakeNextUID()), 
	_pendingDestruction(false),
	_parent(other._parent),
	_children(),
	_name(other._name),
	_relativeTransform(other._relativeTransform),
	_absoluteTransform(other._absoluteTransform)
{
	if (_parent)
		_parent->_children.EmplaceBack(this);
}

WorldObject::~WorldObject()
{
	if (_parent)
		_parent->_children.Remove(IteratorUtils::FirstEqualPosition(_parent->_children.begin(), _parent->_children.end(), this));

	//All children are orphaned as this won't be valid anymore
	for (WorldObject* child : _children)
	{
		child->_parent = nullptr;
		child->_Absolute2Relative();
	}
}

void WorldObject::_Relative2Absolute()
{
	_absoluteTransform = _parent ? (_relativeTransform * _parent->GetAbsoluteTransform()) : _relativeTransform;
	_OnTransformChanged();
}

void WorldObject::_Absolute2Relative()
{
	_relativeTransform = _parent ? _absoluteTransform * _parent->GetAbsoluteTransform().Inverse() : _absoluteTransform;
	_OnTransformChanged();
}

void WorldObject::_OnTransformChanged()
{
	for (WorldObject* child : _children)
		child->_Relative2Absolute();
}

void WorldObject::_OnCreated()
{
	_OnTransformChanged();
}

void WorldObject::_OnCloned()
{
	_OnTransformChanged();
}

void WorldObject::Destroy()
{
	_pendingDestruction = true;

	for (WorldObject* child : _children)
		child->Destroy();
}

void WorldObject::SetParent(WorldObject* parent, bool keepWorldTransform)
{
	if (parent && parent->IsChildOf(*this))
	{
		Debug::Error("WorldObject::SetParent: An object attempted to become a child of one of its own children!");
		return;
	}

	if (_parent) _parent->_children.Remove(IteratorUtils::FirstEqualPosition(_parent->_children.begin(), _parent->_children.end(), this));
	if (_parent = parent) _parent->_children.EmplaceBack(this);

	if (keepWorldTransform) _Absolute2Relative();
	else _Relative2Absolute();
}

void WorldObject::SetName(const Text& name)
{
	_name = name;
}

void WorldObject::MoveAligned(const Vector3& offset)
{
	MoveAbsolute((Vector4(offset, 0.f) * GetAbsoluteTransform().GetMatrix()).GetXYZ() / GetAbsoluteScale());
}

bool WorldObject::IsVisible(const Frustum& view) const
{
	if (const Volume* v = GetVolume()) 
		return v->OverlapsFrustum(GetAbsoluteTransform(), view);

	return false;
}

void WorldObject::Read(ByteReader& reader, ObjectIOContext& ctx)
{
	ctx.objects[ctx.index++] = this;

	uint32 parentIndex = reader.Read_uint32();
	SetParent(parentIndex > 0 ? *ctx.objects.TryGet(parentIndex) : nullptr, false);

	_name = Text::FromString(reader.Read<String>()); //todo- read text, not string
	_relativeTransform.Read(reader);
	
	_Relative2Absolute();
}

void WorldObject::Write(ByteWriter& writer, ObjectIOContext& ctx) const
{
	writer.Write_uint32((_parent && *_parent != *ctx.relParent) ? _parent->_uid : 0);
	
	_name.ToString().Write(writer); //todo- write text, not string
	_relativeTransform.Write(writer);
}

void WorldObject::RenderID(RenderQueue& q, uint32 red) const
{
	uint32 colour[4] = { red, 0, 0, 0xFFFFFFFF };
	q.CreateEntry(ERenderChannels::ALL).AddSetUVec4(RCMDSetUVec4::Type::COLOUR, colour);
	Render(q);
}

bool WorldObject::IsChildOf(const WorldObject& object) const
{
	if (WorldObject* parent = _parent)
		do {
			if (*parent == object)
				return true;
		} while (parent = parent->GetParent());

	return false;
}

//STATIC
const PropertyCollection& WorldObject::_GetNonTransformProperties()
{
	static PropertyCollection properties;

	DO_ONCE_BEGIN;
	properties.Add<uint32>(
		"UID",
		offsetof(WorldObject, _uid),
		PropertyFlags::READONLY
	);
	
	properties.Add(
		"Parent",
		MemberGetter<WorldObject, uint32>(&WorldObject::_P_GetParentUID),
		MemberSetter<WorldObject, uint32>(&WorldObject::_P_SetParentByUID));

	properties.Add(
		"Name",
		MemberGetter<WorldObject, const String&>(&WorldObject::_P_GetName),
		MemberSetter<WorldObject, String>(&WorldObject::_P_SetName));
	DO_ONCE_END;

	return properties;
}

const PropertyCollection& WorldObject::GetProperties()
{
	static PropertyCollection properties(_GetNonTransformProperties());

	DO_ONCE_BEGIN;
	properties.Add(
		"Position",
		MemberGetter<WorldObject, const Vector3&>(&WorldObject::GetRelativePosition),
		MemberSetter<WorldObject, Vector3>(&WorldObject::SetRelativePosition));

	properties.Add(
		"Rotation",
		MemberGetter<WorldObject, const Vector3&>(&WorldObject::_P_GetRotationEuler),
		MemberSetter<WorldObject, Vector3>(&WorldObject::_P_SetRotationEuler));

	properties.Add(
		"Scale",
		MemberGetter<WorldObject, const Vector3&>(&WorldObject::GetRelativeScale),
		MemberSetter<WorldObject, Vector3>(&WorldObject::SetRelativeScale));

	DO_ONCE_END;

	return properties;
}

uint32 WorldObject::_P_GetParentUID() const
{
	return _parent ? _parent->GetUID() : 0;
}

void WorldObject::_P_SetParentByUID(const uint32& uid)
{
	if (uid) SetParent(_world->FindObject(uid), true);
	else SetParent(nullptr, true);
}

const String& WorldObject::_P_GetName() const
{
	return _name.ToString();
}

void WorldObject::_P_SetName(const String& name)
{
	if (_name.ToString() != name)
		_name = Text::FromString(name);
}

const Vector3& WorldObject::_P_GetRotationEuler() const
{
	return GetRelativeRotation().GetEuler();
}

void WorldObject::_P_SetRotationEuler(const Vector3& euler)
{
	SetRelativeRotation(Rotation(euler));
}
