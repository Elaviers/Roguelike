#include "Entity.hpp"
#include "EntityIterator.hpp"
#include "Collider.hpp"
#include "Engine.hpp"
#include "EntCamera.hpp"
#include "GL.hpp"
#include "GLProgram.hpp"
#include "LineSegment.h"
#include "MacroUtilities.hpp"
#include "RaycastResult.hpp"

void Entity::_AddBaseProperties(PropertyCollection &cvars)
{
	cvars.Add(
		"Name",			
		MemberGetter<Entity, const String&>	(&Entity::GetName), 
		MemberSetter<Entity, String>			(&Entity::SetName));

	cvars.Add<uint32>(
		"UID",	
		offsetof(Entity, _uid), 
		PropertyFlags::READONLY);

	cvars.Add(
		"Position",		
		MemberGetter<Transform, const Vector3&>	(&Transform::GetPosition), 
		MemberSetter<Transform, Vector3>			(&Transform::SetPosition),
		offsetof(Entity, _transform));


	cvars.Add(
		"Rotation",		
		MemberGetter<Transform, const Vector3&>	(&Transform::GetRotationEuler),
		MemberSetter<Transform, Vector3>			(&Transform::SetRotationEuler),
		offsetof(Entity, _transform));

	cvars.Add(
		"Scale",
		MemberGetter<Transform, const Vector3&>	(&Transform::GetScale), 
		MemberSetter<Transform, Vector3>			(&Transform::SetScale),
		offsetof(Entity, _transform));
}

const PropertyCollection& Entity::GetProperties()
{
	static PropertyCollection cvars;
	DO_ONCE(_AddBaseProperties(cvars));
	return cvars;
}

Mat4 Entity::GetTransformationMatrix() const
{
	if (_parent)
		return Mat4(_transform.GetTransformationMatrix()) * _parent->GetTransformationMatrix();

	return Mat4(_transform.GetTransformationMatrix());
}

Mat4 Entity::GetInverseTransformationMatrix() const
{
	if (_parent)
		return _parent->GetInverseTransformationMatrix() * _transform.GetInverseTransformationMatrix();

	return _transform.GetInverseTransformationMatrix();
}

void Entity::UpdateAll(float deltaTime)
{
	Update(deltaTime);

	for (size_t i = 0; i < _children.GetSize(); ++i)
		_children[i]->UpdateAll(deltaTime);
}

void Entity::RenderAll(const EntCamera &camera, ERenderChannels channels) const
{
	if (camera.FrustumOverlaps(GetWorldBounds()) || _flags & EFlags::DBG_ALWAYS_DRAW)
		Render(channels);

	for (size_t i = 0; i < _children.GetSize(); ++i)
		_children[i]->RenderAll(camera, channels);
}

void Entity::Delete()
{
	if (Engine::Instance().pObjectTracker)
	{
		Engine::Instance().pObjectTracker->Null(this);
	}


	DeleteChildren();
	SetParent(nullptr);

	if (_dynamic)
	{
		delete this;
		return;
	}
}

//Hierachy

void Entity::SetParent(Entity* parent)
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

Entity* Entity::FindChildWithName(const String& name)
{
	if (_name == name)
		return this;

for (size_t i = 0; i < _children.GetSize(); ++i)
{
	Entity* result = _children[i]->FindChildWithName(name);
	if (result) return result;
}

return nullptr;
}

Entity* Entity::FindChildWithUID(uint32 uid)
{
	if (uid == 0)
		return nullptr;

	if (_uid == uid)
		return this;

	for (size_t i = 0; i < _children.GetSize(); ++i)
	{
		Entity* result = _children[i]->FindChildWithUID(uid);
		if (result) return result;
	}

	return nullptr;
}

//File IO

void Entity::WriteAllToFile(BufferWriter<byte>& buffer, NumberedSet<String>& strings) const
{
	if (_flags & EFlags::SAVEABLE)
	{
		byte id = GetTypeID();
		if (id != 0)
		{
			buffer.Write_byte(id);
			WriteData(buffer, strings);
		}
	}

	for (size_t i = 0; i < _children.GetSize(); ++i)
		_children[i]->WriteAllToFile(buffer, strings);
}

//static
Entity* Entity::CreateFromData(BufferReader<byte>& reader, const NumberedSet<String>& strings)
{
	byte id = reader.Read_byte();
	Entity* obj = Engine::Instance().registry.GetNode(id)->New();
	if (obj)
		obj->ReadData(reader, strings);
	else
		Debug::Error(CSTR("Cannot create Entity with ID ", (int)id));

	return obj;
}

void Entity::WriteData(BufferWriter<byte>& writer, NumberedSet<String>& strings) const
{
	_transform.WriteToBuffer(writer);
	writer.Write_string(_name.GetData());
}

void Entity::ReadData(BufferReader<byte>& reader, const NumberedSet<String>& strings)
{
	_transform.ReadFromBuffer(reader);
	_name = reader.Read_string();
}

//Collision

bool Entity::OverlapsRay(const Ray& ray, RaycastResult& result) const
{
	const Collider* collider = GetCollider();
	if (collider)
		return collider->IntersectsRay(GetWorldTransform(), ray, result);

	return false;
}

EOverlapResult Entity::OverlapsCollider(const Collider& other, const Transform& otherTransform, const Vector3& sweep, Vector3* out_Penetration) const
{
	const Collider* collider = GetCollider();
	if (collider)
	{
		if (sweep.LengthSquared() == 0.f)
			return collider->Overlaps(GetWorldTransform(), other, otherTransform, nullptr, out_Penetration);

		collider->Overlaps(GetWorldTransform(), other, otherTransform, &LineSegment(Vector3(), sweep), out_Penetration);
	}

	return EOverlapResult::SEPERATE;
}

EOverlapResult Entity::AnyPartOverlapsCollider(const Collider& other, const Transform& otherTransform, const Vector3& sweep, Vector3* out_Penetration) const
{
	bool isTouching = false;

	for (ConstEntityIterator it = ConstEntityIterator(this); it.IsValid(); ++it)
	{
		EOverlapResult result = it->OverlapsCollider(other, otherTransform, sweep, out_Penetration);
		if (result == EOverlapResult::OVERLAPPING)
			return EOverlapResult::OVERLAPPING;

		if (!isTouching && result == EOverlapResult::TOUCHING)
			isTouching = true;
	}

	return isTouching ? EOverlapResult::TOUCHING : EOverlapResult::SEPERATE;
}

float Entity::MinimumDistanceToCollider(const Collider& other, const Transform& otherTransform, Vector3& out_PointA, Vector3& out_PointB, const Vector3& sweep) const
{
	const Collider* collider = GetCollider();
	if (collider)
	{
		if (sweep.LengthSquared() == 0.f)
			return collider->MinimumDistanceTo(GetWorldTransform(), other, otherTransform, out_PointA, out_PointB);
	
		return collider->MinimumDistanceTo(GetWorldTransform(), other, otherTransform, out_PointA, out_PointB, &LineSegment(Vector3(), sweep));
	}

	return -1.f;
}

Pair<Vector3> Entity::GetShallowContactPointsWithCollider(float shrink, const Collider& other, const Transform& otherTransform, float otherShrink) const
{
	const Collider* collider = GetCollider();
	if (collider)
		return collider->GetShallowContactPoints(GetWorldTransform(), shrink, other, otherTransform, otherShrink);

	return Pair<Vector3>();
}

Buffer<RaycastResult> Entity::Raycast(const Ray &ray)
{
	RaycastResult result;
	Buffer<RaycastResult> results;

	for (size_t i = 0; i < _children.GetSize(); ++i)
		if (_children[i]->OverlapsRay(ray, result))
		{
			result.object = _children[i];

			size_t index = 0;
			while (index < results.GetSize())
				if (results[index].entryTime > result.entryTime)
					break;
				else index++;

			results.Insert(result, index);
		}

	return results;
}

Buffer<Entity*> Entity::FindOverlappingChildren(const Collider &collider, const Transform &_transform)
{
	Buffer<Entity*> results;

	for (size_t i = 0; i < _children.GetSize(); ++i)
		if (_children[i]->OverlapsCollider(collider, _transform, Vector3(), nullptr) == EOverlapResult::OVERLAPPING)
			results.Add(_children[i]);

	return results;
}

Entity& Entity::operator=(Entity&& other) noexcept
{
	SetParent(other._parent);
	other.SetParent(nullptr);

	for (size_t i = 0; i < other._children.GetSize(); ++i)
		other._children[i]->SetParent(this);

	_transform = other._transform;
	_transform.SetCallback(Callback(this, &Entity::_OnTransformChanged));

	return *this;
}

Bounds Entity::GetWorldBounds(bool noTranslation) const
{
	Bounds b = GetBounds();
	Mat4 wt = GetWorldTransform().GetTransformationMatrix();
	if (noTranslation) wt[3][0] = wt[3][1] = wt[3][2] = 0.f;

	//Yeah, this is not optimised at all...
	Vector3 testPoints[8] = {
		b.min * wt,
		b.max * wt,
		Vector3(b.min.x, b.min.y, b.max.z) * wt,
		Vector3(b.min.x, b.max.y, b.min.z) * wt,
		Vector3(b.min.x, b.max.y, b.max.z) * wt,
		Vector3(b.max.x, b.min.y, b.min.z) * wt,
		Vector3(b.max.x, b.min.y, b.max.z) * wt,
		Vector3(b.max.x, b.max.y, b.min.z) * wt
	};

	Vector3 min = testPoints[0];
	Vector3 max = min;

	for (int i = 0; i < 8; ++i)
	{
		min = Vector3(Maths::Min(min.x, testPoints[i].x), Maths::Min(min.y, testPoints[i].y), Maths::Min(min.z, testPoints[i].z));
		max = Vector3(Maths::Max(max.x, testPoints[i].x), Maths::Max(max.y, testPoints[i].y), Maths::Max(max.z, testPoints[i].z));
	}

	return Bounds(min, max);
}

//
#include "Console.hpp"

void Entity::CMD_List(const Buffer<String>& tokens)
{
	Engine::Instance().pConsole->Print(CSTR(GetClassString(), "\t\t\"", _name, "\"\t\t0x", String::From((int64)_uid, 0, 16), '\n'));

	for (size_t i = 0; i < _children.GetSize(); ++i)
		_children[i]->CMD_List(tokens);
}

void Entity::CMD_Ent(const Buffer<String>& tokens)
{
	if (tokens.GetSize() >= 1)
	{
		Entity* obj = FindChildWithName(tokens[0]);

		if (obj)
		{
			if (tokens.GetSize() >= 2)
			{
				Buffer<String> objTokens(&tokens[1], tokens.GetSize() - 1);
				Engine::Instance().pConsole->Print(obj->GetProperties().HandleCommand(obj, objTokens).GetData());
			}
			else
			{
				const Buffer<Property*> properties = obj->GetProperties().GetAll();

				for (size_t i = 0; i < properties.GetSize(); ++i)
					Engine::Instance().pConsole->Print(CSTR(properties[i]->GetName(), "\t\t", properties[i]->GetTypeString(), "\t\t<", properties[i]->GetAsString(obj), ">\n"));
			}
		}
	}
}
