#pragma once
#include "GizmoComponent.hpp"

class GizmoRing : public GizmoComponent
{
	float _firstAngle;
	Rotation _axis;

	Vector3 _position;
	Vector3 _fv; //oriented along axis
	Vector3 _rv;
	Vector3 _uv;

	bool _GetAngle(const Ray& mouseRay, float &angleOut) const;

public:
	GizmoRing(const Colour& colour, const Rotation& axis) :
		GizmoComponent(colour),
		_firstAngle(0.f),
		_axis(axis)
	{}

	virtual ~GizmoRing() {}

	virtual void SetGizmoTransform(const Transform& transform) override;

	virtual void Render(RenderQueue&) const override;
	virtual void Update(const MouseData& mouseData, const Ray& mouseRay, float& minT) override;
};
