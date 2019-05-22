#include "ObjLight.hpp"
#include "CvarMap.hpp"
#include "Engine.hpp"
#include "GL.hpp"
#include "GLProgram.hpp"
#include "String.hpp"
#include "ShaderChannel.hpp"
#include "ModelManager.hpp"

bool ObjLight::drawLightSources = true;

void ObjLight::GetCvars(CvarMap &cvars)
{
	_AddBaseCvars(cvars);

	cvars.Add("Colour", _colour);
	cvars.Add("Attenuation Radius", _radius);
}

void ObjLight::ToShader(int glArrayIndex)
{
	Mat4 worldTransform = GetWorldTransform().GetTransformationMatrix();

	String arrayElemName = String("Lights[") + String::FromInt(glArrayIndex) + ']';
	glUniform3fv(GLProgram::Current().GetUniformLocation(CSTR(arrayElemName + ".Position")),		1, worldTransform[3]);
	glUniform3fv(GLProgram::Current().GetUniformLocation(CSTR(arrayElemName + ".Colour")),			1, &_colour[0]);
	glUniform1fv(GLProgram::Current().GetUniformLocation(CSTR(arrayElemName + ".Radius")),		1, &_radius);
}

void ObjLight::Render() const
{
	const float colour[4] = {_colour[0], _colour[1], _colour[2], 1.f};

	if (drawLightSources && GLProgram::Current().GetChannels() & ShaderChannel::UNLIT)
	{
		glUniform4fv(GLProgram::Current().GetUniformLocation("Colour"), 1, colour);
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, GetTransformationMatrix());
		Engine::Instance().pModelManager->Cube().Render();
	}
}
