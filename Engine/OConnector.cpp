#include "OConnector.hpp"
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>
#include <ELPhys/CollisionBox.hpp>

void OConnector::Render(RenderQueue& q) const 
{
	static const Matrix4 planeT = Matrix4::RotationY(180.f);

	RenderEntry& e = q.CreateEntry(ERenderChannels::EDITOR);
	e.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
	e.AddSetColour(Colour(0.f, 0.5f, 0.f));
	e.AddSetTransform(planeT * GetAbsoluteTransform().GetMatrix());
	e.AddCommand(RCMDRenderMesh::PLANE);

	e.AddSetColour(Colour(0.5f, 0.f, 0.f));
	e.AddSetTransform(GetAbsoluteTransform().GetMatrix());
	e.AddCommand(RCMDRenderMesh::PLANE);
}

bool OConnector::IsVisible(const Frustum& view) const
{
	Vector3 halfScale = GetAbsoluteScale() / 2.f;
	return view.OverlapsAABB(GetAbsolutePosition() - halfScale, GetAbsolutePosition() + halfScale);
}
