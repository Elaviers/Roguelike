#include "GizmoAxis.hpp"
#include "Gizmo.hpp"
#include <ELGraphics/Colour.hpp>
#include <ELGraphics/TextureManager.hpp>
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>
#include <ELPhys/Collision.hpp>
#include <ELPhys/RaycastResult.hpp>

void GizmoAxis::SetGizmoTransform(const Transform& gizmoTransform)
{
	_position = gizmoTransform.GetPosition();
	_fv = gizmoTransform.GetRotation().GetQuat().Transform(_axis);
	_lineEnd = _position + _fv * _length;
}

void GizmoAxis::Render(RenderQueue& q) const
{
	RenderEntry& e = q.CreateEntry(ERenderChannels::EDITOR);

	if (_canDrag)
		e.AddSetColour(Colour(1.f - _colour.r, 1.f - _colour.g, 1.f - _colour.b));
	else
		e.AddSetColour(_colour);

	e.AddSetLineWidth(2.f);
	e.AddLine(_position, _lineEnd);
}

void GizmoAxis::Update(const MouseData& mouseData, const Ray& mouseRay, float & maxT)
{
	Vector3 line2camera = (mouseRay.origin - _position).Normalise();
	float t = Collision::IntersectRayPlane(mouseRay, _position, Vector3::TripleCross(_fv, line2camera, _fv));
	Vector3 pp = mouseRay.origin + mouseRay.direction * t;

	if (_isDragging)
	{
		if (t != 0.f)
		{
			Vector3 planeDelta = pp - _grabPos;
			_grabPos += _owner->GetMoveFunction()(_fv * _fv.Dot(planeDelta));
		}
	}
	else
	{
		if (t < maxT)
		{
			Vector3 cp = Collision::ClosestPointOnLineSegment(_position, _lineEnd, pp);
			float dist2 = (cp - pp).LengthSquared();

			if (dist2 <= 0.05f * 0.05f)
			{
				maxT = t;
				_grabPos = pp;
				_canDrag = true;
				return;
			}
		}

		_canDrag = false;
	}
}
