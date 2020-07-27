#include "RenderCommand.hpp"
#include "MeshManager.hpp"
#include "TextureManager.hpp"
#include <ELSys/GLProgram.hpp>

void RCMDSetBool::Execute(RenderContext& ctx) const
{
	switch (_type)
	{
	case Type::FONT:
		GLProgram::Current().SetBool(DefaultUniformVars::boolIsFont, _value);
		break;
	}
}

void RCMDSetFloat::Execute(RenderContext& ctx) const
{
	switch (_type)
	{
	case Type::LINE_WIDTH:
		glLineWidth(_value);
		break;
	}
}

void RCMDSetTransform::Execute(RenderContext& ctx) const
{
	GLProgram::Current().SetMatrix4(DefaultUniformVars::mat4Model, _matrix);
}

void RCMDSetUVOffset::Execute(RenderContext& ctx) const
{
	GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVOffset, _uvOffset);
}

void RCMDSetUVScale::Execute(RenderContext& ctx) const
{
	GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, _uvScale);
}

void RCMDSetColour::Execute(RenderContext& ctx) const
{
	if (_bBlend)
	{
		GLProgram::Current().SetBool(DefaultUniformVars::boolBlend, true);
		GLProgram::Current().SetVec4(DefaultUniformVars::vec4BlendFrom, _colour);
		GLProgram::Current().SetVec4(DefaultUniformVars::vec4BlendTo, _secondaryColour);
	}
	else
	{
		GLProgram::Current().SetBool(DefaultUniformVars::boolBlend, false);
		GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, _colour);
	}
}

void RCMDSetTexture::Execute(RenderContext& ctx) const
{
	glActiveTexture(GL_TEXTURE0 + _unit);

	if (_type)
	{
		switch ((RCMDSetTexture::Type)_type)
		{
		case Type::WHITE:
			ctx.textureManager->White()->Bind(_unit);
			break;
		case Type::BLACK:
			ctx.textureManager->Black()->Bind(_unit);
			break;
		case Type::GREY:
			ctx.textureManager->Grey()->Bind(_unit);
			break;
		case Type::UNIT_NORMAL:
			ctx.textureManager->UnitNormal()->Bind(_unit);
			break;
		}
	}
	else
		glBindTexture(GL_TEXTURE_2D, _tex);
}

void RCMDSetCubemap::Execute(RenderContext& ctx) const
{
	glActiveTexture(GL_TEXTURE0 + _unit);
	glBindTexture(GL_TEXTURE_CUBE_MAP, _tex);
}

const RCMDRenderMesh RCMDRenderMesh::LINE(_Type::LINE);
const RCMDRenderMesh RCMDRenderMesh::PLANE(_Type::PLANE);
const RCMDRenderMesh RCMDRenderMesh::CUBE(_Type::CUBE);
const RCMDRenderMesh RCMDRenderMesh::INV_CUBE(_Type::INV_CUBE);

void RCMDRenderMesh::Execute(RenderContext& ctx) const
{
	if (_type)
	{
		switch ((RCMDRenderMesh::_Type)_type)
		{
		case _Type::LINE:
			ctx.meshManager->Line()->Render();
			break;
		case _Type::PLANE:
			ctx.meshManager->Plane()->Render();
			break;
		case _Type::CUBE:
			ctx.meshManager->Cube()->Render();
			break;
		case _Type::INV_CUBE:
			ctx.meshManager->InverseCube()->Render();
			break;
		}
	}
	else
	{
		_mesh->Render();
	}
}

void RCMDSetSkinningMatrices::Execute(RenderContext& ctx) const
{
	if (_ref)
		glUniformMatrix4fv(GLProgram::Current().GetUniformLocation(DefaultUniformVars::Matrix4aBones),
			(GLsizei)_ref->GetSize(), GL_FALSE, (float*)_ref->Data());
	else
		glUniformMatrix4fv(GLProgram::Current().GetUniformLocation(DefaultUniformVars::Matrix4aBones),
			(GLsizei)_matrices.GetSize(), GL_FALSE, (float*)_matrices.Data());
}

void RCMDLight::Execute(RenderContext& ctx) const
{
	int glArrayIndex = ctx.submittedLightCount++;
	String arrayElemName = String("Lights[") + String::From(glArrayIndex) + ']';
	glUniform3fv(GLProgram::Current().GetUniformLocation(CSTR(arrayElemName, ".Position")), 1, _position.GetData());
	glUniform3fv(GLProgram::Current().GetUniformLocation(CSTR(arrayElemName, ".Colour")), 1, _colour.GetData());
	glUniform1fv(GLProgram::Current().GetUniformLocation(CSTR(arrayElemName, ".Radius")), 1, &_radius);
}

void RCMDSetDepthFunc::Execute(RenderContext& ctx) const
{
	glDepthFunc(_depthFunc);
}

const RCMDSetDepthFunc RCMDSetDepthFunc::NEVER(GL_NEVER);
const RCMDSetDepthFunc RCMDSetDepthFunc::LESS(GL_LESS);
const RCMDSetDepthFunc RCMDSetDepthFunc::EQUAL(GL_EQUAL);
const RCMDSetDepthFunc RCMDSetDepthFunc::LEQUAL(GL_LEQUAL);
const RCMDSetDepthFunc RCMDSetDepthFunc::GREATER(GL_GREATER);
const RCMDSetDepthFunc RCMDSetDepthFunc::NOTEQUAL(GL_NOTEQUAL);
const RCMDSetDepthFunc RCMDSetDepthFunc::GEQUAL(GL_GEQUAL);
const RCMDSetDepthFunc RCMDSetDepthFunc::ALWAYS(GL_ALWAYS);
