#pragma once
#include "DebugObject.h"
#include "ObjCamera.h"

class DebugFrustum : public DebugObject
{
	Vector3 _origin;
	Vector3 _near[4];
	Vector3 _far[4];

public:
	DebugFrustum(float time = 0.f, Vector4 colour = Vector4(1.f, 0.f, 0.f)) : DebugObject(time, colour) {}
	virtual ~DebugFrustum() {}

	virtual void Render() const override;

	static DebugFrustum FromCamera(const ObjCamera&);

	SIZEOF_DEF;
};
