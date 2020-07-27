#pragma once
#include "DebugObject.hpp"

class Projection;
class Transform;

class DebugFrustum : public DebugObject
{
	Vector3 _origin;
	Vector3 _near[4];
	Vector3 _far[4];

public:
	DebugFrustum(float time = 0.f, const Colour& colour = Colour::Red, float lineWidth = 2, float fadeTime = 0.f) : DebugObject(time, fadeTime, colour, lineWidth) {}
	virtual ~DebugFrustum() {}

	virtual void Render(RenderQueue&) const override;

	static DebugFrustum FromCamera(const Transform&, const Projection&, float time = 0.f, const Colour& colour = Colour::Red, float lineWidth = 2.f, float fadeTime = 0.f);

	SIZEOF_DEF;
};
