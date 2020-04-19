#include "EntLight.hpp"
#include "Engine.hpp"
#include "GL.hpp"
#include "GLProgram.hpp"
#include "MacroUtilities.hpp"
#include "ModelManager.hpp"
#include "String.hpp"
#include "TextureManager.hpp"

int EntLight::_frameLightCounter;

Vector3 EntLight::_editorBoxExtent(.1f, .1f, .1f);

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

void EntLight::ToShader(int glArrayIndex) const
{
	Mat4 worldTransform = GetWorldTransform().GetTransformationMatrix();

	String arrayElemName = String("Lights[") + String::From(glArrayIndex) + ']';
	glUniform3fv(GLProgram::Current().GetUniformLocation(CSTR(arrayElemName, ".Position")),	1, worldTransform[3]);
	glUniform3fv(GLProgram::Current().GetUniformLocation(CSTR(arrayElemName, ".Colour")),	1, _colour.GetData());
	glUniform1fv(GLProgram::Current().GetUniformLocation(CSTR(arrayElemName, ".Radius")),	1, &_radius);
}

void EntLight::Update(float deltaTime)
{
	EntLight::_frameLightCounter = 0;
}

void EntLight::Render(ERenderChannels channels) const
{
	if (channels & ERenderChannels::PRE_RENDER && EntLight::_frameLightCounter < GLProgram::Current().GetMaxLightCount())
		ToShader(EntLight::_frameLightCounter++);

	if (EntLight::drawLightSources && Engine::Instance().pTextureManager && channels & ERenderChannels::EDITOR)
	{
		const float colour[4] = { _colour.x, _colour.y, _colour.z, 1.f };
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

//static
void EntLight::FinaliseLightingForFrame()
{
	for (int i = _frameLightCounter; i < GLProgram::Current().GetMaxLightCount(); ++i)
	{
		GLProgram::Current().SetFloat(CSTR("Lights[", i, "].Radius"), 0.f);
	}
}
