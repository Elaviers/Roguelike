#pragma once
#include "GizmoComponent.hpp"

class GizmoPlane : public GizmoComponent
{
	Vector3 _grabOffset;

	Setter<const Vector3&> _move;

protected:
	virtual void _OnTransformChanged() override;

public:
	GizmoPlane(const Colour& colour, Setter<const Vector3&> move) :
		GizmoComponent(colour),
		_move(move)
	{}

	virtual ~GizmoPlane() {}

	virtual void Render(RenderQueue&) const override;
	virtual void Update(const Ray& mouseRay, float& maxT) override;
};
