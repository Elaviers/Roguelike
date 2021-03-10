#include "Entity.hpp"
#include "EntityIterator.hpp"
#include "Registry.hpp"
#include <ELCore/Context.hpp>
#include <ELMaths/Frustum.hpp>
#include <ELMaths/LineSegment.hpp>
#include <ELCore/Tracker.hpp>

void Entity::_OnTransformChanged()
{
	_wtValid = _wbValid = false;
	onTransformChanged();

	for (Entity* child : _children)
		child->_OnTransformChanged();
}

void Entity::_OnChildChanged()
{
	onChildChanged();

	if (_parent)
		_parent->_OnChildChanged();
}

void Entity::_AddBaseProperties(PropertyCollection &cvars)
{
	cvars.Add(
		"Name",			
		MemberGetter<Entity, const String&>	(&Entity::GetName), 
		MemberSetter<Entity, String>		(&Entity::SetName));

	cvars.Add<uint32>(
		"UID",	
		offsetof(Entity, _uid), 
		PropertyFlags::READONLY);

	cvars.Add(
		"Position",		
		MemberGetter<Transform, const Vector3&>	(&Transform::GetPosition), 
		MemberSetter<Transform, Vector3>		(&Transform::SetPosition),
		offsetof(Entity, _transform));


	cvars.Add(
		"Rotation",		
		MemberGetter<Transform, const Vector3&>	(&Transform::GetRotationEuler),
		MemberSetter<Transform, Vector3>		(&Transform::SetRotationEuler),
		offsetof(Entity, _transform));

	cvars.Add(
		"Scale",
		MemberGetter<Transform, const Vector3&>	(&Transform::GetScale), 
		MemberSetter<Transform, Vector3>		(&Transform::SetScale),
		offsetof(Entity, _transform));
}

const PropertyCollection& Entity::GetProperties()
{
	static PropertyCollection cvars;
	DO_ONCE(_AddBaseProperties(cvars));
	return cvars;
}

Matrix4 Entity::GetTransformationMatrix() const
{
	if (_parent)
		return Matrix4(_transform.GetTransformationMatrix()) * _parent->GetTransformationMatrix();

	return Matrix4(_transform.GetTransformationMatrix());
}

Matrix4 Entity::GetInverseTransformationMatrix() const
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

void Entity::RenderAll(RenderQueue& q, const Frustum& cameraFrustum) const
{
	Bounds b = GetWorldBounds();

	if ((_flags & EFlags::ALWAYS_DRAW) != EFlags::NONE || cameraFrustum.OverlapsAABB(b.min, b.max))
		Render(q);

	for (size_t i = 0; i < _children.GetSize(); ++i)
		_children[i]->RenderAll(q, cameraFrustum);
}

void Entity::Delete(const Context& ctx)
{
	Tracker<Entity>* entTracker = ctx.GetPtr<Tracker<Entity>>(false);
	if (entTracker)
		entTracker->Null(this);

	DeleteChildren(ctx);
	SetParent(nullptr);

	if (dynamicBuffer)
	{
		delete this;
		return;
	}
}

//Hierachy

void Entity::SetParent(Entity* parent)
{
	if (parent == _parent)
		return;

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

	_wbValid = false;
}

Entity* Entity::FindChild(const String& name)
{
	if (_name == name)
		return this;

	for (size_t i = 0; i < _children.GetSize(); ++i)
	{
		Entity* result = _children[i]->FindChild(name);
		if (result) return result;
	}

	return nullptr;
}

Entity* Entity::FindChild(uint32 uid)
{
	if (uid == 0)
		return nullptr;

	if (_uid == uid)
		return this;

	for (size_t i = 0; i < _children.GetSize(); ++i)
	{
		Entity* result = _children[i]->FindChild(uid);
		if (result) return result;
	}

	return nullptr;
}

//File IO

void Entity::WriteAllToFile(ByteWriter& buffer, NumberedSet<String>& strings, const Context& ctx) const
{
	if ((_flags & EFlags::SAVEABLE) != EFlags::NONE)
	{
		byte id = GetTypeID();
		if (id != 0)
		{
			buffer.Write_byte(id);
			WriteData(buffer, strings, ctx);
		}
	}

	for (size_t i = 0; i < _children.GetSize(); ++i)
		_children[i]->WriteAllToFile(buffer, strings, ctx);
}

//static
Entity* Entity::CreateFromData(ByteReader& reader, const NumberedSet<String>& strings, const Context& ctx)
{
	byte id = reader.Read_byte();
	Registry<Entity>* registry = ctx.GetPtr<Registry<Entity>>();
	Entity* obj = registry->GetNode(id)->New();
	if (obj)
		obj->ReadData(reader, strings, ctx);
	else
		Debug::Error(CSTR("Cannot create Entity with ID ", (int)id));

	return obj;
}

void Entity::WriteData(ByteWriter& writer, NumberedSet<String>& strings, const Context& ctx) const
{
	_transform.Write(writer);
	_name.Write(writer);
}

void Entity::ReadData(ByteReader& reader, const NumberedSet<String>& strings, const Context& ctx)
{
	_transform.Read(reader);
	_name.Read(reader);
}

//Collision

bool Entity::OverlapsRay(const Ray& ray, ECollisionChannels rayChannels, RaycastResult& result) const
{
	const Collider* collider = GetCollider();
	if (collider)
		return collider->IntersectsRay(GetWorldTransform(), ray, rayChannels, result);

	return false;
}

EOverlapResult Entity::OverlapsCollider(const Collider& other, const Transform& otherTransform, const Vector3& sweep, Vector3* out_Penetration) const
{
	const Collider* collider = GetCollider();
	if (collider)
	{
		if (sweep.LengthSquared() == 0.f)
			return collider->Overlaps(GetWorldTransform(), other, otherTransform, nullptr, out_Penetration);

		LineSegment sweepLine(Vector3(), sweep);
		collider->Overlaps(GetWorldTransform(), other, otherTransform, &sweepLine, out_Penetration);
	}

	return EOverlapResult::SEPERATE;
}

EOverlapResult Entity::AnyPartOverlapsCollider(const Collider& other, const Transform& otherTransform, const Vector3& sweep, Vector3* out_Penetration) const
{
	bool isTouching = false;

	Vector3 colliderCentre = otherTransform.GetPosition();
	float colliderRadius = other.GetMaximumRadius() * Maths::Max(otherTransform.GetScale().GetData(), 3);

	for (ConstEntityIterator it = ConstEntityIterator(this); it.IsValid(); ++it)
	{
		if (it->GetCollider())
		{
			Vector3 itCentre = it->GetWorldTransform().GetPosition();
			float itRadius = it->GetCollider()->GetMaximumRadius() * Maths::Max(it->GetWorldTransform().GetScale().GetData(), 3);

			if ((colliderCentre - itCentre).LengthSquared() <= colliderRadius * colliderRadius + itRadius * itRadius)
			{
				EOverlapResult result = it->OverlapsCollider(other, otherTransform, sweep, out_Penetration);
				if (result == EOverlapResult::OVERLAPPING)
					return EOverlapResult::OVERLAPPING;

				if (!isTouching && result == EOverlapResult::TOUCHING)
					isTouching = true;
			}
		}
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
	
		LineSegment sweepLine(Vector3(), sweep);
		return collider->MinimumDistanceTo(GetWorldTransform(), other, otherTransform, out_PointA, out_PointB, &sweepLine);
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

Buffer<RaycastResult> Entity::Raycast(const Ray &ray, ECollisionChannels rayChannels)
{
	RaycastResult result;
	Buffer<RaycastResult> results;

	for (size_t i = 0; i < _children.GetSize(); ++i)
		if (_children[i]->OverlapsRay(ray, rayChannels, result))
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
	_transform.SetCallback(Callback(*this, &Entity::_OnTransformChanged));

	_worldTransform = other._worldTransform;
	_wtValid = other._wtValid;

	_worldBounds = other._worldBounds;
	_wbValid = other._wbValid;
	return *this;
}

Bounds Entity::GetWorldBounds() const
{
	if (_wbValid)
		return _worldBounds;

	Bounds b = GetBounds();
	Matrix4 wt = GetWorldTransform().GetTransformationMatrix();
	
	//Yeah, this is not optimised at all...
	Vector3 testPoints[8] = {
		(Vector4(b.min, 1.f) * wt).GetXYZ(),
		(Vector4(b.max, 1.f) * wt).GetXYZ(),
		(Vector4(b.min.x, b.min.y, b.max.z, 1.f) * wt).GetXYZ(),
		(Vector4(b.min.x, b.max.y, b.min.z, 1.f) * wt).GetXYZ(),
		(Vector4(b.min.x, b.max.y, b.max.z, 1.f) * wt).GetXYZ(),
		(Vector4(b.max.x, b.min.y, b.min.z, 1.f) * wt).GetXYZ(),
		(Vector4(b.max.x, b.min.y, b.max.z, 1.f) * wt).GetXYZ(),
		(Vector4(b.max.x, b.max.y, b.min.z, 1.f) * wt).GetXYZ()
	};

	Vector3 min = testPoints[0];
	Vector3 max = min;

	for (int i = 0; i < 8; ++i)
	{
		min = Vector3(Maths::Min(min.x, testPoints[i].x), Maths::Min(min.y, testPoints[i].y), Maths::Min(min.z, testPoints[i].z));
		max = Vector3(Maths::Max(max.x, testPoints[i].x), Maths::Max(max.y, testPoints[i].y), Maths::Max(max.z, testPoints[i].z));
	}

	//todo: eugghghhhhhhhhhhhhhhh const cast euuuuuughhhhhhhhhh
	const_cast<Entity*>(this)->_worldBounds = Bounds::FromMinMax(min, max);
	const_cast<Entity*>(this)->_wbValid = true;
	return _worldBounds;
}

//
#include "Console.hpp"

void Entity::CMD_List(const Buffer<String>& tokens, const Context& ctx)
{
	Console* console = ctx.GetPtr<Console>();
	console->Print(CSTR(GetClassString(), "\t\t\"", _name, "\"\t\t0x", String::FromInt(_uid, 0, 16), '\n'));

	for (size_t i = 0; i < _children.GetSize(); ++i)
		_children[i]->CMD_List(tokens, ctx);
}

void Entity::CMD_Ent(const Buffer<String>& tokens, const Context& ctx)
{
	if (tokens.GetSize() >= 1)
	{
		Entity* obj = FindChild(tokens[0]);

		if (obj)
		{
			Console* console = ctx.GetPtr<Console>();

			if (tokens.GetSize() >= 2)
			{
				Buffer<String> objTokens(&tokens[1], tokens.GetSize() - 1);
				console->Print(obj->GetProperties().HandleCommand(obj, objTokens, ctx).GetData());
			}
			else
			{
				const Buffer<Property*> properties = obj->GetProperties().GetAll();

				for (size_t i = 0; i < properties.GetSize(); ++i)
					console->Print(CSTR(properties[i]->GetName(), "\t\t", properties[i]->GetTypeName(), "\t\t<", properties[i]->GetAsString(obj, ctx), ">\n"));
			}
		}
	}
}
