#include "ObjBox.h"
#include "DrawUtils.h"
#include "Engine.h"
#include "GLProgram.h"
#include "ShaderChannel.h"

void ObjBox::Render() const
{
	if (Engine::Instance().pModelManager && GLProgram::Current().GetChannels() == ShaderChannel::UNLIT)
	{
		GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, _colour);
		DrawUtils::DrawBox(*Engine::Instance().pModelManager, _min, _max);
	}
}

void ObjBox::_UpdateMinMax()
{
	if (_p1[0] < _p2[0])
	{
		_min[0] = _p1[0];
		_max[0] = _p2[0];
	}
	else
	{
		_max[0] = _p1[0];
		_min[0] = _p2[0];
	}

	if (_p1[1] < _p2[1])
	{
		_min[1] = _p1[1];
		_max[1] = _p2[1];
	}
	else
	{
		_max[1] = _p1[1];
		_min[1] = _p2[1];
	}

	if (_p1[2] < _p2[2])
	{
		_min[2] = _p1[2];
		_max[2] = _p2[2];
	}
	else
	{
		_max[2] = _p1[2];
		_min[2] = _p2[2];
	}
}
