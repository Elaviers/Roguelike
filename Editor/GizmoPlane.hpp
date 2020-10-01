#pragma once
#include "GizmoComponent.hpp"

class GizmoPlane : public GizmoComponent
{
	Vector3 _grabPos;

	Transform _relativeTransform;
	Transform _transform;
public:
	GizmoPlane(const Colour& colour, const Transform& relativeTransform) :
		GizmoComponent(colour),
		_relativeTransform(relativeTransform),
		_transform(relativeTransform)
	{}

	virtual ~GizmoPlane() {}

	virtual void SetGizmoTransform(const Transform& transform) override;

	virtual void Render(RenderQueue&) const override;
	virtual void Update(const MouseData& mouseData, const Ray& mouseRay, float& maxT) override;
};
