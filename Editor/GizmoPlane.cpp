#include "GizmoPlane.hpp"
#include <Engine/Collision.hpp>
#include <Engine/Engine.hpp>
#include <Engine/GLProgram.hpp>
#include <Engine/ModelManager.hpp>

void GizmoPlane::_OnTransformChanged()
{

}

void GizmoPlane::Draw() const
{
	if (Engine::Instance().pModelManager && Engine::Instance().pTextureManager)
	{
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, transform.GetTransformationMatrix());
		
		if (_canDrag)
			GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, Colour(1.f - _colour.r, 1.f - _colour.g, 1.f - _colour.b));
		else
			GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, _colour);

		glDisable(GL_CULL_FACE);
		Engine::Instance().pModelManager->Plane()->Render();
		glEnable(GL_CULL_FACE);
	}
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
		Vector3 cv = transform.GetRightVector() * transform.GetScale()[0] + transform.GetUpVector() * transform.GetScale()[1];

		float t = Collision::IntersectRayRect(ray, 
			transform.GetPosition(), 
			transform.GetForwardVector(), 
			transform.GetRightVector() * (transform.GetScale()[0] / 2.f),
			transform.GetUpVector() * (transform.GetScale()[1] / 2.f));

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
