#include "EntConnector.hpp"
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderQueue.hpp>
#include <ELPhys/CollisionBox.hpp>

void EntConnector::_OnTransformChanged()
{
	if (!_updatingTransform)
	{
		float hw = GetRelativeScale().x / 2.f;
		float hh = GetRelativeScale().y / 2.f;
		float hd = GetRelativeScale().z / 2.f;

		_point1 = Vector3(GetRelativePosition().x - hw, GetRelativePosition().y - hh, GetRelativePosition().z - hd);
		_point2 = Vector3(GetRelativePosition().x + hw, GetRelativePosition().y + hh, GetRelativePosition().z + hd);
	}
}

void EntConnector::_OnPointChanged()
{
	float x = (_point1.x + _point2.x) / 2.f;
	float y = (_point1.y + _point2.y) / 2.f;
	float z = (_point1.z + _point2.z) / 2.f;
	float w = Maths::Abs(_point1.x - _point2.x);
	float h = Maths::Abs(_point1.y - _point2.y);
	float d = Maths::Abs(_point1.z - _point2.z);

	_updatingTransform = true;
	SetRelativePosition(Vector3(x, y, z));
	SetRelativeScale(Vector3(w, h, d));
	_updatingTransform = false;
}

void EntConnector::Render(RenderQueue& q) const {
	static const Matrix4 planeT = Matrix4::RotationY(180.f);

	RenderEntry& e = q.NewDynamicEntry(ERenderChannels::EDITOR);
	e.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
	e.AddSetColour(Colour(0.f, 0.5f, 0.f));
	e.AddSetTransform(planeT * GetTransformationMatrix());
	e.AddCommand(RCMDRenderMesh::PLANE);

	e.AddSetColour(Colour(0.5f, 0.f, 0.f));
	e.AddSetTransform(GetTransformationMatrix());
	e.AddCommand(RCMDRenderMesh::PLANE);

	e.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
	e.AddSetColour(Colour(0.f, 0.5f, 0.f));
	e.AddSetLineWidth(2.f);
	e.AddBox(_point1, _point2);
}
