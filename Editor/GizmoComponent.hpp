#pragma once
#include <ELGraphics/Colour.hpp>
#include <ELMaths/Ray.hpp>
#include <ELMaths/Transform.hpp>

class Gizmo;
struct MouseData;
class RenderQueue;

class GizmoComponent
{
protected:
	friend Gizmo;

	Gizmo* _owner;

	Colour _colour;

	bool _canDrag;
	bool _isDragging;

	GizmoComponent(const Colour& colour) : 
		_colour(colour), 
		_canDrag(false),
		_isDragging(false)
	{}

public:
	virtual ~GizmoComponent() {}

	virtual void SetGizmoTransform(const Transform& transform) {};
	virtual void SetObjectTransform(const Transform& transform) {};

	virtual void Render(RenderQueue&) const {}

	/*
		Will not consider collisions past maxT
		if successful, maxT is set to the entry time
	*/
	virtual void Update(const MouseData& mouseData, const Ray& mouseRay, float& maxT) {}

	bool MouseDown() { 
		if (_canDrag)
		{
			_isDragging = true;
			return true;
		}
		
		return false; 
	}

	void MouseUp() { _isDragging = false; }
};
