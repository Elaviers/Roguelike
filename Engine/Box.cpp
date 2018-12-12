#include "Box.h"
#include "DrawUtils.h"
#include "Engine.h"

void Box::Render() const
{
	if (Engine::modelManager)
	{
		DrawUtils::DrawBox(*Engine::modelManager, _min, _max);
	}
}

void Box::_UpdateMinMax()
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
