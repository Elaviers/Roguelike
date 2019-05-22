#include "GameObject.hpp"
#include "Collider.hpp"
#include "CvarMap.hpp"
#include "Engine.hpp"
#include "GL.hpp"
#include "GLProgram.hpp"
#include "ObjCamera.hpp"
#include "RaycastResult.hpp"

void GameObject::_AddBaseCvars(CvarMap &cvars)
{
	cvars.Add("UID", const_cast<uint32&>(_uid), PropertyFlags::READONLY);
	cvars.Add("Position",	Getter<const Vector3&>(&_transform, &Transform::GetPosition), Setter<Vector3>(&_transform, &Transform::SetPosition));
	cvars.Add("Rotation",	Getter<const Vector3&>(&_transform, &Transform::GetRotationEuler), Setter<Vector3>(&_transform, &Transform::SetRotationEuler));
	cvars.Add("Scale",		Getter<const Vector3&>(&_transform, &Transform::GetScale), Setter<Vector3>(&_transform, &Transform::SetScale));
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
		return _transform.GetInverseTransformationMatrix() * _parent->GetInverseTransformationMatrix();

	return _transform.GetInverseTransformationMatrix();
}

void GameObject::Render(const ObjCamera &camera) const
{
	if (camera.FrustumOverlaps(GetWorldBounds()) || _flags & FLAG_DBG_ALWAYS_DRAW)
		Render();

	for (uint32 i = 0; i < _children.GetSize(); ++i)
		_children[i]->Render(camera);
}

//File IO

void GameObject::WriteAllToFile(BufferIterator<byte> &buffer, NumberedSet<String> &strings) const
{
	if (_flags & FLAG_SAVEABLE)
	{
		byte id = Engine::Instance().registry.GetFirstCompatibleID(this);
		if (id != 0)
		{
			buffer.Write_byte(id);
			WriteToFile(buffer, strings);
		}
	}

	for (uint32 i = 0; i < _children.GetSize(); ++i)
		_children[i]->WriteAllToFile(buffer, strings);
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
