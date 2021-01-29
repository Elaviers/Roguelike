#include "GizmoRing.hpp"
#include "Gizmo.hpp"
#include "MouseData.hpp"
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>
#include <ELPhys/Collision.hpp>

bool GizmoRing::_GetAngle(const Ray& ray, float& angleOut) const
{
	float t = Collision::IntersectRayPlane(ray, _position, _fv);
	if (t)
	{
		//Direction from centre to cursor
		Vector3 dir = (ray.origin + ray.direction * t - _position).Normalised();

		float x = _rv.Dot(dir);
		float y = -_uv.Dot(dir);

		angleOut = Maths::RadiansToDegrees(Maths::ArcTan2(y, x));
		return true;
	}

	return false;
}

void GizmoRing::SetGizmoTransform(const Transform& gizmoTransform)
{
	_position = gizmoTransform.GetPosition();

	Quaternion q = _axis.GetQuat() * gizmoTransform.GetRotation().GetQuat();

	_fv = q.GetForwardVector();
	_rv = q.GetRightVector();
	_uv = q.GetUpVector();
}

void GizmoRing::Render(RenderQueue& q) const
{
	RenderEntry& e = q.CreateEntry(ERenderChannels::EDITOR);

	if (_canDrag)
		e.AddSetColour(Colour(1.f - _colour.r, 1.f - _colour.g, 1.f - _colour.b));
	else
		e.AddSetColour(_colour);
	
	e.AddSetLineWidth(2.f);
	e.AddRing(_position, _fv, 1.f, 32);
}

void GizmoRing::Update(const MouseData& mouseData, const Ray& ray, float& maxT)
{
	if (_isDragging)
	{
		float angle;
		if (_GetAngle(ray, angle))
		{
			_firstAngle += _owner->GetRotateFunction()(_fv, Maths::SmallestAngularDiff(angle, _firstAngle));
		}
	}
	else
	{
		float t = Collision::IntersectRayRing(ray, _position, _fv, 0.9f, 1.1f);

		if (t && t < maxT)
		{
			maxT = t;
			_canDrag = _GetAngle(ray, _firstAngle);
		}
		else
			_canDrag = false;
	}
}
