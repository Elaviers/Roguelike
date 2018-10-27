#include "GameObject.h"
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
	GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, transform.MakeTransformationMatrix());
	//															TODO - ANYTHING BUT THIS ^
}
