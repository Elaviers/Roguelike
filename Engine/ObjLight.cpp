#include "ObjLight.hpp"
#include "CvarMap.hpp"
#include "Engine.hpp"
#include "GL.hpp"
#include "GLProgram.hpp"
#include "ModelManager.hpp"
#include "String.hpp"
#include "TextureManager.hpp"

const Vector3 ObjLight::_editorBoxExtent(.1f, .1f, .1f);
const ColliderBox ObjLight::_lightCollider(COLL_EDITOR, ObjLight::_editorBoxExtent);

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

	String arrayElemName = String("Lights[") + String::From(glArrayIndex) + ']';
	glUniform3fv(GLProgram::Current().GetUniformLocation(CSTR(arrayElemName, ".Position")),		1, worldTransform[3]);
	glUniform3fv(GLProgram::Current().GetUniformLocation(CSTR(arrayElemName, ".Colour")),			1, &_colour[0]);
	glUniform1fv(GLProgram::Current().GetUniformLocation(CSTR(arrayElemName, ".Radius")),			1, &_radius);
}

void ObjLight::Render(EnumRenderChannel channels) const
{
	const float colour[4] = {_colour[0], _colour[1], _colour[2], 1.f};

	if (drawLightSources && Engine::Instance().pTextureManager && channels & RenderChannel::EDITOR)
	{
		Engine::Instance().pTextureManager->White()->Bind(0);
		glUniform4fv(GLProgram::Current().GetUniformLocation("Colour"), 1, colour);
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, Matrix::Scale(_editorBoxExtent * 2.f) * GetTransformationMatrix());
		Engine::Instance().pModelManager->Cube().Render();
	}
}

void ObjLight::WriteData(BufferWriter<byte>& writer, NumberedSet<String>& strings) const
{
	GameObject::WriteData(writer, strings);

	writer.Write_float(_radius);
	writer.Write_vector3(_colour);
}

void ObjLight::ReadData(BufferReader<byte>& reader, const NumberedSet<String>& strings)
{
	GameObject::ReadData(reader, strings);

	_radius = reader.Read_float();
	_colour = reader.Read_vector3();
}
