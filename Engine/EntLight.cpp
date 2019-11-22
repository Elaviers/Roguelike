#include "EntLight.hpp"
#include "Engine.hpp"
#include "GL.hpp"
#include "GLProgram.hpp"
#include "MacroUtilities.hpp"
#include "ModelManager.hpp"
#include "String.hpp"
#include "TextureManager.hpp"

const Vector3 EntLight::_editorBoxExtent(.1f, .1f, .1f);
const ColliderBox EntLight::_lightCollider(COLL_EDITOR, EntLight::_editorBoxExtent);

bool EntLight::drawLightSources = true;

const PropertyCollection& EntLight::GetProperties()
{
	static PropertyCollection cvars;

	DO_ONCE_BEGIN;
	_AddBaseProperties(cvars);

	cvars.Add<Vector4>(
		"Colour", 
		offsetof(EntLight, _colour));

	cvars.Add<float>(
		"Radius", 
		offsetof(EntLight, _radius));
	DO_ONCE_END;

	return cvars;
}

void EntLight::ToShader(int glArrayIndex)
{
	Mat4 worldTransform = GetWorldTransform().GetTransformationMatrix();

	String arrayElemName = String("Lights[") + String::From(glArrayIndex) + ']';
	glUniform3fv(GLProgram::Current().GetUniformLocation(CSTR(arrayElemName, ".Position")),		1, worldTransform[3]);
	glUniform3fv(GLProgram::Current().GetUniformLocation(CSTR(arrayElemName, ".Colour")),			1, &_colour[0]);
	glUniform1fv(GLProgram::Current().GetUniformLocation(CSTR(arrayElemName, ".Radius")),			1, &_radius);
}

void EntLight::Render(EnumRenderChannel channels) const
{
	const float colour[4] = {_colour[0], _colour[1], _colour[2], 1.f};

	if (drawLightSources && Engine::Instance().pTextureManager && channels & RenderChannel::EDITOR)
	{
		Engine::Instance().pTextureManager->White()->Bind(0);
		glUniform4fv(GLProgram::Current().GetUniformLocation("Colour"), 1, colour);
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, Matrix::Scale(_editorBoxExtent * 2.f) * GetTransformationMatrix());
		Engine::Instance().pModelManager->Cube()->Render();
	}
}

void EntLight::WriteData(BufferWriter<byte>& writer, NumberedSet<String>& strings) const
{
	Entity::WriteData(writer, strings);

	writer.Write_float(_radius);
	writer.Write_vector3(_colour);
}

void EntLight::ReadData(BufferReader<byte>& reader, const NumberedSet<String>& strings)
{
	Entity::ReadData(reader, strings);

	_radius = reader.Read_float();
	_colour = reader.Read_vector3();
}
