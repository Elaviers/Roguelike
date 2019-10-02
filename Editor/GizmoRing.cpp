#include "GizmoRing.hpp"
#include <Engine/DrawUtils.hpp>
#include <Engine/Engine.hpp>
#include <Engine/GLProgram.hpp>

bool GizmoRing::_GetAngle(const Ray& ray, float& angleOut) const
{
	float t = Collision::IntersectRayPlane(ray, transform.GetPosition(), transform.GetForwardVector());
	if (t)
	{
		//Direction from centre to cursor
		Vector3 dir = (ray.origin + ray.direction * t - transform.GetPosition()).Normalised();
		Vector3 rv = transform.GetRightVector();
		Vector3 uv = transform.GetUpVector();

		float x = Vector3::Dot(rv, dir);
		float y = -Vector3::Dot(uv, dir);

		angleOut = Maths::ArcTangentDegrees2(y, x);
		return true;
	}

	return false;
}

void GizmoRing::Draw() const
{
	if (Engine::Instance().pModelManager && Engine::Instance().pTextureManager)
	{
		if (_canDrag)
			GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, Colour(1.f - _colour.r, 1.f - _colour.g, 1.f - _colour.b));
		else
			GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, _colour);
	
		DrawUtils::DrawRing(*Engine::Instance().pModelManager, transform.GetPosition(), transform.GetForwardVector(), 1.f, 32);
	}
}

void GizmoRing::Update(const Ray& ray, float& minT)
{
	if (_isDragging)
	{
		float angle;
		if (_GetAngle(ray, angle))
		{
			_firstAngle += _rotate(transform.GetForwardVector(), Maths::SmallestAngularDiff(angle, _firstAngle));
		}
	}
	else
	{
		float t = Collision::IntersectRayRing(ray, transform.GetPosition(), transform.GetForwardVector(), 0.9f, 1.1f);

		if (t && t < minT)
		{
			minT = t;
			_canDrag = _GetAngle(ray, _firstAngle);
		}
		else
			_canDrag = false;
	}
}
