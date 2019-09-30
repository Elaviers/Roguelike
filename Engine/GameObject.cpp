#include "GameObject.hpp"
#include "Collider.hpp"
#include "Engine.hpp"
#include "GL.hpp"
#include "GLProgram.hpp"
#include "MacroUtilities.hpp"
#include "ObjCamera.hpp"
#include "RaycastResult.hpp"

void GameObject::_AddBaseProperties(PropertyCollection &cvars)
{
	cvars.Add(
		"Name",			
		MemberGetter<GameObject, const String&>	(&GameObject::GetName), 
		MemberSetter<GameObject, String>			(&GameObject::SetName));

	cvars.Add<uint32>(
		"UID",	
		offsetof(GameObject, _uid), 
		PropertyFlags::READONLY);

	cvars.Add(
		"Position",		
		MemberGetter<Transform, const Vector3&>	(&Transform::GetPosition), 
		MemberSetter<Transform, Vector3>			(&Transform::SetPosition),
		offsetof(GameObject, _transform));


	cvars.Add(
		"Rotation",		
		MemberGetter<Transform, const Vector3&>	(&Transform::GetRotationEuler),
		MemberSetter<Transform, Vector3>			(&Transform::SetRotationEuler),
		offsetof(GameObject, _transform));

	cvars.Add(
		"Scale",
		MemberGetter<Transform, const Vector3&>	(&Transform::GetScale), 
		MemberSetter<Transform, Vector3>			(&Transform::SetScale),
		offsetof(GameObject, _transform));
}

const PropertyCollection& GameObject::GetProperties()
{
	static PropertyCollection cvars;
	DO_ONCE(_AddBaseProperties(cvars));
	return cvars;
}

//Public

Mat4 GameObject::GetTransformationMatrix() const
{
	if (_parent)
		return Mat4(_transform.GetTransformationMatrix()) * _parent->GetTransformationMatrix();

	return Mat4(_transform.GetTransformationMatrix());
}

Mat4 GameObject::GetInverseTransformationMatrix() const
{
	if (_parent)
		return _parent->GetInverseTransformationMatrix() * _transform.GetInverseTransformationMatrix();

	return _transform.GetInverseTransformationMatrix();
}

void GameObject::Render(const ObjCamera &camera, EnumRenderChannel channels) const
{
	if (camera.FrustumOverlaps(GetWorldBounds()) || _flags & FLAG_DBG_ALWAYS_DRAW)
		Render(channels);

	for (uint32 i = 0; i < _children.GetSize(); ++i)
		_children[i]->Render(camera, channels);
}

void GameObject::Delete()
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

//File IO

void GameObject::WriteAllToFile(BufferWriter<byte> &buffer, NumberedSet<String> &strings) const
{
	if (_flags & FLAG_SAVEABLE)
	{
		auto id = GetTypeID();
		if (id != 0)
		{
			buffer.Write_byte(id);
			WriteData(buffer, strings);
		}
	}

	for (uint32 i = 0; i < _children.GetSize(); ++i)
		_children[i]->WriteAllToFile(buffer, strings);
}

//static
GameObject* GameObject::CreateFromData(BufferReader<byte>& reader, const NumberedSet<String>& strings)
{
	byte id = reader.Read_byte();
	GameObject* obj = Engine::Instance().registry.GetNode(id)->New();
	if (obj)
		obj->ReadData(reader, strings);
	else 
		Debug::Error(CSTR("Cannot create gameobject with ID ", (int)id));

	return obj;
}

void GameObject::WriteData(BufferWriter<byte>& writer, NumberedSet<String>& strings) const
{
	_transform.WriteToBuffer(writer);
	writer.Write_string(_name.GetData());
}

void GameObject::ReadData(BufferReader<byte>& reader, const NumberedSet<String>& strings)
{
	_transform.ReadFromBuffer(reader);
	_name = reader.Read_string();
}

//Collision

bool GameObject::OverlapsRay(const Ray &ray, RaycastResult &result) const
{
	if (this->GetCollider())
		return this->GetCollider()->IntersectsRay(ray, result, GetWorldTransform());

	return false;
}

bool GameObject::OverlapsCollider(const Collider &other, const Transform &otherTransform) const
{
	if (this->GetCollider())
		return this->GetCollider()->Overlaps(other, otherTransform, GetWorldTransform());

	return false;
}

Buffer<RaycastResult> GameObject::Raycast(const Ray &ray)
{
	RaycastResult result;
	Buffer<RaycastResult> results;

	for (uint32 i = 0; i < _children.GetSize(); ++i)
		if (_children[i]->OverlapsRay(ray, result))
		{
			result.object = _children[i];

			uint32 index = 0;
			while (index < results.GetSize())
				if (results[index].entryTime > result.entryTime)
					break;
				else index++;

			results.Insert(result, index);
		}

	return results;
}

Buffer<GameObject*> GameObject::FindOverlaps(const Collider &collider, const Transform &_transform)
{
	Buffer<GameObject*> results;

	for (uint32 i = 0; i < _children.GetSize(); ++i)
		if (_children[i]->OverlapsCollider(collider, _transform))
			results.Add(_children[i]);

	return results;
}
