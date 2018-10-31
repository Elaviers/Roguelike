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

Mat4 GameObject::GetTransformationMatrix()
{
	if (_parent)
		return Mat4(transform.GetTransformationMatrix()) * _parent->GetTransformationMatrix();

	return Mat4(transform.GetTransformationMatrix());
}

Mat4 GameObject::MakeInverseTransformationMatrix() const
{
	if (_parent)
		return transform.MakeInverseTransformationMatrix() * _parent->MakeInverseTransformationMatrix();

	return transform.MakeInverseTransformationMatrix();
}

void GameObject::ApplyTransformToShader() const
{
	if (_parent)
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, const_cast<Transform&>(transform).GetTransformationMatrix() * _parent->GetTransformationMatrix());
	//											TODO - ANYTHING BUT THIS ^

	else
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, const_cast<Transform&>(transform).GetTransformationMatrix());
	//																Yuck yuck yuck yuck yuck
}

bool GameObject::Raycast(const Ray &ray, RaycastResult &result) const
{
	if (this->GetCollider())
		return this->GetCollider()->IntersectsRay(ray, result, transform);

	return false;
}

bool GameObject::Overlaps(const Collider &other, const Transform &otherTransform) const
{
	if (this->GetCollider())
		return this->GetCollider()->Overlaps(other, otherTransform, transform);

	return false;
}
