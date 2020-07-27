#include "DebugObject.hpp"

void DebugObject::Update(float deltaTime)
{
	_fadeTime += deltaTime;
	
	if (_fadeTime > 0.f)
	{
		((Vector4&)_colour)[3] -= deltaTime * _fadeSpeed;
	}

	_timeRemaining -= deltaTime;
}
