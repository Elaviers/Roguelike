#include "GameObject.h"
#include "GL.h"
#include "GLProgram.h"

Mat4 GameObject::MakeInverseTransformationMatrix()
{
	if (_parent)
		return transform.MakeInverseTransformationMatrix() * _parent->MakeInverseTransformationMatrix();

	return transform.MakeInverseTransformationMatrix();
}

//Public

Mat4 GameObject::MakeTransformationMatrix()
{
	if (_parent)
		return transform.MakeTransformationMatrix() * _parent->MakeTransformationMatrix();

	return transform.MakeTransformationMatrix();
}

void GameObject::ApplyTransformToShader()
{
	glUniformMatrix4fv(
		GLProgram::Current().GetUniformLocation("M_Model"),
		1,
		GL_FALSE,
		&MakeTransformationMatrix()[0][0]
	);
}
