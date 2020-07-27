#include "DebugFrustum.hpp"
#include "RenderCommand.hpp"
#include "RenderQueue.hpp"
#include <ELMaths/Transform.hpp>
#include <ELMaths/Projection.hpp>

void DebugFrustum::Render(RenderQueue& q) const
{
	RenderEntry& e = q.NewDynamicEntry(ERenderChannels::EDITOR);
	e.AddSetColour(_colour);
	e.AddSetLineWidth(_lineWidth);

	for (int i = 0; i < 4; ++i) e.AddLine(_origin, _far[i]);

	e.AddLine(_near[0], _near[1]);
	e.AddLine(_near[1], _near[3]);
	e.AddLine(_near[3], _near[2]);
	e.AddLine(_near[2], _near[0]);

	e.AddLine(_far[0], _far[1]);
	e.AddLine(_far[1], _far[3]);
	e.AddLine(_far[3], _far[2]);
	e.AddLine(_far[2], _far[0]);
}

//static
DebugFrustum DebugFrustum::FromCamera(const Transform& transform, const Projection &projection, float time, const Colour& colour, float lineWidth, float fadeTime)
{
	DebugFrustum frustum(time, colour, lineWidth, fadeTime);

	frustum._origin = transform.GetPosition();
		
	Ray ray = projection.ScreenToWorld(transform, Vector2(-0.5f, -0.5f));
	frustum._near[0] = ray.origin + ray.direction * projection.GetNear();
	frustum._far[0] = ray.origin + ray.direction * projection.GetFar();

	ray = projection.ScreenToWorld(transform, Vector2(0.5f, -0.5f));
	frustum._near[1] = ray.origin + ray.direction * projection.GetNear();
	frustum._far[1] = ray.origin + ray.direction * projection.GetFar();

	ray = projection.ScreenToWorld(transform, Vector2(-0.5f, 0.5f));
	frustum._near[2] = ray.origin + ray.direction * projection.GetNear();
	frustum._far[2] = ray.origin + ray.direction * projection.GetFar();

	ray = projection.ScreenToWorld(transform, Vector2(0.5f, 0.5f));
	frustum._near[3] = ray.origin + ray.direction * projection.GetNear();
	frustum._far[3] = ray.origin + ray.direction * projection.GetFar();

	return frustum;
}
