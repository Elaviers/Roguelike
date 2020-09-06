#pragma once
#include "GizmoComponent.hpp"
#include <ELCore/FunctionPointer.hpp>

/*
	GizmoAxis

	Line thingy
*/

class GizmoAxis : public GizmoComponent
{
	Vector3 _position;
	Vector3 _axis;
	float _length;

	Vector3 _grabOffset;
	Vector3 _fv;
	Vector3 _lineEnd;

public:
	GizmoAxis(const Colour& colour, const Vector3& axis) : 
		GizmoComponent(colour),
		_axis(axis),
		_length(1.f)
	{}

	virtual ~GizmoAxis() {}

	virtual void SetGizmoTransform(const Transform& transform) override;

	virtual void Render(RenderQueue&) const override;
	virtual void Update(const MouseData& mouseData, const Ray &mouseRay, float &maxT) override;
};
