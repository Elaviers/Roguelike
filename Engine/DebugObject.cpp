#include "DebugObject.hpp"
#include "GLProgram.hpp"

void DebugObject::_PreRender() const
{
	GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, _colour);
	glLineWidth(2);
}

void DebugObject::Update(float deltaTime)
{
	_timeRemaining -= deltaTime;
}
