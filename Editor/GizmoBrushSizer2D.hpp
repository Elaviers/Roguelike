#pragma once
#include "Gizmo.hpp"

class GizmoBrushSizer2D : public GizmoComponent
{
protected:
	E2DBrushEdge _edge;

	Vector3 _lineA;
	Vector3 _lineB;

	Vector3 _normal;
	Vector3 _edgePoint;

public:
	GizmoBrushSizer2D(const Colour& colour, E2DBrushEdge edge) :
		GizmoComponent(colour),
		_edge(edge)
	{}

	virtual ~GizmoBrushSizer2D() {}

	virtual void SetObjectTransform(const Transform& transform) override;

	virtual void Render(RenderQueue&) const override;
	virtual void Update(const MouseData& mouseData, const Ray& mouseRay, float& maxT) override;
};
