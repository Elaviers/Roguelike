#include "GeoIsoTile.hpp"
#include <ELGraphics/RenderQueue.hpp>

SharedPointer<const Mesh> GeoIsoTile::_mesh;

void GeoIsoTile::Render(RenderQueue& q) const
{
	if (_material && _mesh)
	{
		RenderEntry& e = q.NewDynamicEntry(ERenderChannels::UNLIT);
		e.AddSetTransform(_renderTransform.GetTransformationMatrix());
		e.AddSetColour(); 
		_material->Apply(e);
		e.AddSetUVOffset();
		e.AddSetUVScale();
		e.AddRenderMesh(*_mesh);
	}
}

void GeoIsoTile::SetTransform(const Vector3& position, const Vector2& size)
{

	/* -- SET TRANSFORM FOR PLANE MESH  -- */
	/*
	float pitch = Maths::ArcTangent(1.f / Maths::SQRT2_F);

	//Assuming sprite lines are at a slope of 1/2 instead of tan(30), stretch vertically
	float stretch = Maths::TangentDegrees(30.f) / 0.5f;

	//Multiply by root 2 because we want a hypotenuse length
	float w = size.x * Maths::SQRT2_F * (66.f / 64.f); //todo: the 66/64 is temporary! It's because the textures have a 1px overlap right now
	float h = size.y * stretch * Maths::SQRT2_F * (66.f / 64.f);

	//        --Hypotenuse to centre bottom--
	float m = Maths::Sine(pitch) * h * 0.5f / Maths::SQRT2_F;

	_renderTransform.SetRotation(Vector3(-35.264f, 45.f, 0.f));
	_renderTransform.SetPosition(position + Vector3(m, Maths::Cosine(pitch) * h * 0.5f, m));
	_renderTransform.SetScale(Vector3(w, h, 1.f));
	*/

	//Since we are using a custom mesh, the above isn't necessary at the moment
	_renderTransform.SetPosition(position);
	_renderTransform.SetScale(Vector3(size.x, size.y, size.x));
}
