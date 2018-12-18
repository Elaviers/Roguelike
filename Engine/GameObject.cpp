#include "GameObject.h"
#include "Collider.h"
#include "GL.h"
#include "GLProgram.h"
#include "ObjectProperties.h"

void GameObject::_AddBaseProperties(ObjectProperties &properties)
{
	properties.Add<Transform, Vector3>("Position", &transform, &Transform::GetPosition, &Transform::SetPosition);
	properties.Add<Transform, Vector3>("Rotation", &transform, &Transform::GetRotation, &Transform::SetRotation);
	properties.Add<Transform, Vector3>("Scale",	   &transform, &Transform::GetScale, &Transform::SetScale);
}

//Public

Mat4 GameObject::GetTransformationMatrix() const
{
	if (_parent)
		return Mat4(transform.GetTransformationMatrix()) * _parent->GetTransformationMatrix();

	return Mat4(transform.GetTransformationMatrix());
}

Mat4 GameObject::GetInverseTransformationMatrix() const
{
	if (_parent)
		return transform.GetInverseTransformationMatrix() * _parent->GetInverseTransformationMatrix();

	return transform.GetInverseTransformationMatrix();
}

bool GameObject::Raycast(const Ray &ray, RaycastResult &result) const
{
	if (this->GetCollider())
		return this->GetCollider()->IntersectsRay(ray, result, GetWorldTransform());

	return false;
}

bool GameObject::Overlaps(const Collider &other, const Transform &otherTransform) const
{
	if (this->GetCollider())
		return this->GetCollider()->Overlaps(other, otherTransform, GetWorldTransform());

	return false;
}
