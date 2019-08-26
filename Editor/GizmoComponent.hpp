#pragma once
#include <Engine/Ray.hpp>
#include <Engine/Transform.hpp>

class GizmoComponent
{
protected:
	Vector4 _colour;

	bool _canDrag;
	bool _isDragging;
	
	virtual void _OnTransformChanged() {}

	GizmoComponent(Vector4 colour) : 
		_colour(colour), 
		_canDrag(false),
		_isDragging(false),
		transform(Callback(this, &GizmoComponent::_OnTransformChanged)) 
	{}

public:
	Transform transform;

	virtual ~GizmoComponent() {}

	virtual void Draw() const {}

	/*
		Will not consider collisions past maxT
		if successful, maxT is set to the entry time
	*/
	virtual void Update(const Ray& mouseRay, float& maxT) {}

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
