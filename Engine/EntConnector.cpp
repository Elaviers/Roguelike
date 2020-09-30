#include "EntConnector.hpp"
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>
#include <ELPhys/CollisionBox.hpp>

void EntConnector::Render(RenderQueue& q) const {
	static const Matrix4 planeT = Matrix4::RotationY(180.f);

	RenderEntry& e = q.CreateEntry(ERenderChannels::EDITOR);
	e.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
	e.AddSetColour(Colour(0.f, 0.5f, 0.f));
	e.AddSetTransform(planeT * GetTransformationMatrix());
	e.AddCommand(RCMDRenderMesh::PLANE);

	e.AddSetColour(Colour(0.5f, 0.f, 0.f));
	e.AddSetTransform(GetTransformationMatrix());
	e.AddCommand(RCMDRenderMesh::PLANE);
}
