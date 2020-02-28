#include "UIColour.hpp"
#include "GLProgram.hpp"

void UIColour::ApplyToShader() const
{
	if (_isBlend)
	{
		GLProgram::Current().SetBool(DefaultUniformVars::boolBlend, true);
		GLProgram::Current().SetVec4(DefaultUniformVars::vec4BlendFrom, _blendFrom);
		GLProgram::Current().SetVec4(DefaultUniformVars::vec4BlendTo, _blendTo);
	}
	else
	{
		GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, _colour);
	}
}
