#include "DebugFrustum.h"
#include "DrawUtils.h"
#include "Engine.h"
#include "Ray.h"

using namespace DrawUtils;

void DebugFrustum::Render() const
{
	if (Engine::modelManager)
	{
		_PreRender();

		const ModelManager &mm = *Engine::modelManager;

		for (int i = 0; i < 4; ++i) DrawLine(mm, _origin, _far[i]);

		DrawLine(mm, _near[0], _near[1]);
		DrawLine(mm, _near[1], _near[3]);
		DrawLine(mm, _near[3], _near[2]);
		DrawLine(mm, _near[2], _near[0]);

		DrawLine(mm, _far[0], _far[1]);
		DrawLine(mm, _far[1], _far[3]);
		DrawLine(mm, _far[3], _far[2]);
		DrawLine(mm, _far[2], _far[0]);
	}
}

//static

DebugFrustum DebugFrustum::FromCamera(const ObjCamera &camera)
{
	DebugFrustum frustum;

	frustum._origin = camera.transform.GetPosition();
		
	Ray ray = camera.ScreenCoordsToRay(Vector2(-0.5f, -0.5f));
	frustum._near[0] = ray.origin + ray.direction * camera.GetNear();
	frustum._far[0] = ray.origin + ray.direction * camera.GetFar();

	ray = camera.ScreenCoordsToRay(Vector2(0.5f, -0.5f));
	frustum._near[1] = ray.origin + ray.direction * camera.GetNear();
	frustum._far[1] = ray.origin + ray.direction * camera.GetFar();

	ray = camera.ScreenCoordsToRay(Vector2(-0.5f, 0.5f));
	frustum._near[2] = ray.origin + ray.direction * camera.GetNear();
	frustum._far[2] = ray.origin + ray.direction * camera.GetFar();

	ray = camera.ScreenCoordsToRay(Vector2(0.5f, 0.5f));
	frustum._near[3] = ray.origin + ray.direction * camera.GetNear();
	frustum._far[3] = ray.origin + ray.direction * camera.GetFar();

	return frustum;
}
