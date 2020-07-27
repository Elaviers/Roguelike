#pragma once
#include "DebugObject.hpp"

class DebugLine : public DebugObject
{
	Vector3 _a;
	Vector3 _b;

public:
	DebugLine(const Vector3& a, const Vector3& b, float time, const Colour& colour = Colour::Red, float lineWidth = 2, float fadeTime = 0.f) 
		: DebugObject(time, fadeTime, colour, lineWidth), _a(a), _b(b) {}
	virtual ~DebugLine() {}

	virtual void Render(RenderQueue&) const;

	SIZEOF_DEF;
};
