#pragma once
#include "DebugObject.hpp"
#include "ObjCamera.hpp"

class DebugFrustum : public DebugObject
{
	Vector3 _origin;
	Vector3 _near[4];
	Vector3 _far[4];

public:
	DebugFrustum(float time = 0.f, Colour colour = Colour::Red) : DebugObject(time, colour) {}
	virtual ~DebugFrustum() {}

	virtual void Render() const override;

	static DebugFrustum FromCamera(const ObjCamera&);

	SIZEOF_DEF;
};
