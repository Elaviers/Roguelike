#include "DebugObject.hpp"
#include "GLProgram.hpp"

void DebugObject::_PreRender() const
{
	GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, _colour);
	glLineWidth(_lineWidth);
}

#include "Debug.hpp"

void DebugObject::Update(float deltaTime)
{
	_fadeTime += deltaTime;
	
	if (_fadeTime > 0.f)
	{
		((Vector4&)_colour)[3] -= deltaTime * _fadeSpeed;
	}

	_timeRemaining -= deltaTime;
}
