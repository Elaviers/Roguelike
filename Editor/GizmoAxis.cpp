#include "GizmoAxis.hpp"
#include <ELGraphics/Colour.hpp>
#include <ELGraphics/TextureManager.hpp>
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>
#include <ELPhys/Collision.hpp>
#include <ELPhys/RaycastResult.hpp>

void GizmoAxis::_OnTransformChanged()
{
	_fv = transform.GetForwardVector();

	_collisionBox->SetTransform(Transform((2.f * transform.GetPosition() + _fv * _length) / 2.f, transform.GetRotation(), Vector3(0.1f, 0.1f, _length / 2.f)));
}

void GizmoAxis::Render(RenderQueue& q) const
{
	RenderEntry& e = q.NewDynamicEntry(ERenderChannels::EDITOR);

	if (_canDrag)
		e.AddSetColour(Colour(1.f - _colour.r, 1.f - _colour.g, 1.f - _colour.b));
	else
		e.AddSetColour(_colour);

	e.AddSetLineWidth(2.f);
	e.AddLine(transform.GetPosition(), transform.GetPosition() + _fv * _length);
}

void GizmoAxis::Update(const Ray& mouseRay, float &minT)
{
	if (_isDragging)
	{
		float t = Collision::IntersectRayPlane(mouseRay, transform.GetPosition(), transform.GetRightVector());
		if (t != 0.f)
		{
			Vector3 planeDelta = mouseRay.origin + mouseRay.direction * t - (transform.GetPosition() + _grabOffset);

			//Snap to axis
			Vector3 fv = transform.GetForwardVector();
			_move.TryCall(_fv * planeDelta.Dot(_fv));
		}
	}
	else
	{
		RaycastResult rr;
		if (_collider.IntersectsRay(Transform(), mouseRay, ECollisionChannels::ALL, rr) && rr.entryTime < minT)
		{
			minT = rr.entryTime;

			_grabOffset = mouseRay.origin + mouseRay.direction * rr.entryTime - transform.GetPosition();
			
			_canDrag = true;
		}
		else
			_canDrag = false;
	}
}
