#include "GizmoAxis.hpp"
#include <Engine/Colour.hpp>
#include <Engine/DrawUtils.hpp>
#include <Engine/Engine.hpp>
#include <Engine/GLProgram.hpp>
#include <Engine/RaycastResult.hpp>
#include <Engine/TextureManager.hpp>

void GizmoAxis::_OnTransformChanged()
{
	_fv = transform.GetForwardVector();

	_collisionBox->SetTransform(Transform((2.f * transform.GetPosition() + _fv * _length) / 2.f, transform.GetRotation(), Vector3(0.1f, 0.1f, _length / 2.f)));
}

void GizmoAxis::Draw() const
{
	if (Engine::Instance().pModelManager && Engine::Instance().pTextureManager)
	{
		if (_canDrag)
			GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, Colour(1.f - _colour.r, 1.f - _colour.g, 1.f - _colour.b));
		else
			GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, _colour);

		DrawUtils::DrawLine(
			*Engine::Instance().pModelManager, 
			transform.GetPosition(), 
			transform.GetPosition() + _fv * _length);
	}
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
		if (_collider.IntersectsRay(Transform(), mouseRay, rr) && rr.entryTime < minT)
		{
			minT = rr.entryTime;

			_grabOffset = mouseRay.origin + mouseRay.direction * rr.entryTime - transform.GetPosition();
			
			_canDrag = true;
		}
		else
			_canDrag = false;
	}
}
