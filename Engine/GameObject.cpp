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

//Hierachy

void GameObject::SetParent(GameObject* parent)
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

GameObject* GameObject::FindByName(const String& name)
{
	if (_name == name)
		return this;

	for (size_t i = 0; i < _children.GetSize(); ++i)
	{
		GameObject* result = _children[i]->FindByName(name);
		if (result) return result;
	}

	return nullptr;
}

GameObject* GameObject::FindByUID(uint32 uid)
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

GameObject& GameObject::operator=(GameObject&& other) noexcept
{
	SetParent(other._parent);
	other.SetParent(nullptr);

	for (uint32 i = 0; i < other._children.GetSize(); ++i)
		other._children[i]->SetParent(this);

	_transform = other._transform;
	_transform.SetCallback(Callback(this, &GameObject::_OnTransformChanged));

	return *this;
}

Bounds GameObject::GetWorldBounds(bool noTranslation) const
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

//
#include "Console.hpp"

void GameObject::CMD_List(const Buffer<String>& tokens)
{
	Engine::Instance().pConsole->Print(CSTR(GetClassString(), "\t\t\"", _name, "\"\t\t0x", String::From((int64)_uid, 0, 16), '\n'));

	for (size_t i = 0; i < _children.GetSize(); ++i)
		_children[i]->CMD_List(tokens);
}

void GameObject::CMD_Ent(const Buffer<String>& tokens)
{
	if (tokens.GetSize() >= 2)
	{
		GameObject* obj = FindByName(tokens[0]);

		if (obj)
		{
			Buffer<String> objTokens(&tokens[1], tokens.GetSize() - 1);
			Engine::Instance().pConsole->Print(obj->GetProperties().HandleCommand(obj, objTokens).GetData());
		}
	}
}

void GameObject::CMD_ListProperties(const Buffer<String>& tokens)
{
	if (tokens.GetSize() >= 1)
	{
		GameObject* obj = FindByName(tokens[0]);

		if (obj)
		{
			const Buffer<Property*> properties = obj->GetProperties().GetAll();

			for (size_t i = 0; i < properties.GetSize(); ++i)
				Engine::Instance().pConsole->Print(CSTR(properties[i]->GetName(), "\t\t", properties[i]->GetTypeString(), "\t\t<", properties[i]->GetAsString(obj), ">\n"));
		}
	}
}
