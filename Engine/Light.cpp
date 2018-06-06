#include "Light.h"
#include "GL.h"
#include "GLProgram.h"
#include "String.h"

void Light::ToShader(int glArrayIndex)
{
	Mat4 worldTransform = MakeTransformationMatrix();

	String arrayElemName = String("Lights[") + String::Convert(glArrayIndex) + ']';
	glUniform3fv(GLProgram::Current().GetUniformLocation(CSTR(arrayElemName + ".Position")),		1, worldTransform[3]);
	glUniform3fv(GLProgram::Current().GetUniformLocation(CSTR(arrayElemName + ".Colour")),			1, &_colour[0]);
	glUniform1fv(GLProgram::Current().GetUniformLocation(CSTR(arrayElemName + ".Radius")),		1, &_radius);
}

#include "CubeRenderer.h"

void Light::DebugRender(CubeRenderer &cubeRenderer)
{
	float colour[4] = {_colour[0], _colour[1], _colour[2], 1.f};

	glUniform4fv(GLProgram::Current().GetUniformLocation("Colour"), 1, colour);
	GameObject::ApplyTransformToShader();
	cubeRenderer.Render();
}
