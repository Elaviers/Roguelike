#include "GizmoPlane.hpp"
#include <Engine/ModelManager.hpp>
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderEntry.hpp>
#include <ELGraphics/RenderQueue.hpp>
#include <ELPhys/Collision.hpp>

void GizmoPlane::_OnTransformChanged()
{

}

void GizmoPlane::Render(RenderQueue& q) const
{
	RenderEntry& e = q.NewDynamicEntry(ERenderChannels::EDITOR);

	if (_canDrag)
		e.AddSetColour(Colour(1.f - _colour.r, 1.f - _colour.g, 1.f - _colour.b));
	else
		e.AddSetColour(_colour);

	e.AddSetTransform(transform.GetTransformationMatrix());
	e.AddCommand(RCMDRenderMesh::PLANE);
}

void GizmoPlane::Update(const Ray& ray, float& minT)
{
	if (_isDragging)
	{
		float t = Collision::IntersectRayPlane(ray, transform.GetPosition(), transform.GetForwardVector());
		if (t)
		{
			_move.TryCall(ray.origin + ray.direction * t - (transform.GetPosition() + _grabOffset));
		}
	}
	else
	{
		Vector3 cv = transform.GetRightVector() * transform.GetScale().x + transform.GetUpVector() * transform.GetScale().y;

		float t = Collision::IntersectRayRect(ray, 
			transform.GetPosition(), 
			transform.GetForwardVector(), 
			transform.GetRightVector() * (transform.GetScale().x / 2.f),
			transform.GetUpVector() * (transform.GetScale().y / 2.f));

		if (t && t < minT)
		{
			minT = t;
			_grabOffset = ray.origin + ray.direction * t - transform.GetPosition();
			_canDrag = true;
		}
		else
			_canDrag = false;
	}
}
