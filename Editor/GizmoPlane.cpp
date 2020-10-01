#include "GizmoPlane.hpp"
#include "Gizmo.hpp"
#include "MouseData.hpp"
#include <Engine/ModelManager.hpp>
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderEntry.hpp>
#include <ELGraphics/RenderQueue.hpp>
#include <ELPhys/Collision.hpp>

void GizmoPlane::SetGizmoTransform(const Transform& gizmoTransform)
{
	_transform = _relativeTransform * gizmoTransform;
}

void GizmoPlane::Render(RenderQueue& q) const
{
	RenderEntry& e = q.CreateEntry(ERenderChannels::EDITOR);

	if (_canDrag)
		e.AddSetColour(Colour(1.f - _colour.r, 1.f - _colour.g, 1.f - _colour.b));
	else
		e.AddSetColour(_colour);

	e.AddSetTransform(_transform.GetTransformationMatrix());
	e.AddCommand(RCMDRenderMesh::PLANE);
	e.AddSetTransform(Matrix4::RotationY(180.f) * _transform.GetTransformationMatrix());
	e.AddCommand(RCMDRenderMesh::PLANE);
}

void GizmoPlane::Update(const MouseData& mouseData, const Ray& ray, float& maxT)
{
	if (_isDragging)
	{
		float t = Collision::IntersectRayPlane(ray, _transform.GetPosition(), _transform.GetForwardVector());
		if (t)
		{
			Vector3 moveDelta = (ray.origin + ray.direction * t) - _grabPos;
			_grabPos += _owner->GetMoveFunction()(moveDelta);
		}
	}
	else
	{
		float t = Collision::IntersectRayRect(ray, 
			_transform.GetPosition(),
			_transform.GetForwardVector(),
			_transform.GetRightVector() * (_transform.GetScale().x / 2.f),
			_transform.GetUpVector() * (_transform.GetScale().y / 2.f));

		if (t && t < maxT)
		{
			maxT = t;
			_grabPos = ray.origin + ray.direction * t;
			_canDrag = true;
		}
		else
			_canDrag = false;
	}
}
