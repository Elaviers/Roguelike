#include "GameObject.h"
#include "GL.h"
#include "GLProgram.h"

void GameObject::_AddBaseProperties(ObjectProperties &properties)
{
	properties.Add<Transform, Vector3>("Position", &transform, &Transform::GetPosition, &Transform::SetPosition);
	properties.Add<Transform, Vector3>("Rotation", &transform, &Transform::GetRotation, &Transform::SetRotation);
	properties.Add<Transform, Vector3>("Scale",	   &transform, &Transform::GetScale, &Transform::SetScale);
}

Mat4 GameObject::MakeInverseTransformationMatrix() const
{
	if (_parent)
		return transform.MakeInverseTransformationMatrix() * _parent->MakeInverseTransformationMatrix();

	return transform.MakeInverseTransformationMatrix();
}

//Public

Mat4 GameObject::MakeTransformationMatrix() const
{
	if (_parent)
		return transform.MakeTransformationMatrix() * _parent->MakeTransformationMatrix();

	return transform.MakeTransformationMatrix();
}

void GameObject::ApplyTransformToShader() const
{
	glUniformMatrix4fv(
		GLProgram::Current().GetUniformLocation("M_Model"),
		1,
		GL_FALSE,
		&MakeTransformationMatrix()[0][0]
	);
}
