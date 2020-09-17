#include "GeoIsoTile.hpp"
#include "EGeometryID.hpp"
#include "TileManager.hpp"
#include <ELGraphics/RenderQueue.hpp>

SharedPointer<const Mesh> GeoIsoTile::_mesh;

const byte GeoIsoTile::TypeID = (byte)EGeometryID::ISO_TILE;

void GeoIsoTile::Render(RenderQueue& q) const
{
	if (_tile && _mesh && _tile->GetMaterial())
	{
		RenderEntry& e = q.NewDynamicEntry(ERenderChannels::UNLIT);
		e.AddSetTransform(_renderTransform.GetTransformationMatrix());
		e.AddSetColour(); 
		_tile->GetMaterial()->Apply(e);
		e.AddSetUVOffset();
		e.AddSetUVScale();
		e.AddRenderMesh(*_mesh);
	}
}

void GeoIsoTile::WriteData(ByteWriter& data, NumberedSet<String>& strings, const Context& ctx) const
{
	data.Write<Vector3>(_renderTransform.GetPosition());
	data.Write<Vector3>(_renderTransform.GetScale());

	data.Write_uint16(_tile ? strings.Add(ctx.GetPtr<TileManager>()->FindNameOf(_tile.Ptr())) : 0);
}

void GeoIsoTile::ReadData(ByteReader& data, const NumberedSet<String>& strings, const Context& ctx)
{
	_renderTransform.SetPosition(data.Read<Vector3>());
	_renderTransform.SetScale(data.Read<Vector3>());

	const String* tname = strings.Get(data.Read_uint16());
	if (tname)
		_tile = ctx.GetPtr<TileManager>()->Get(*tname, ctx);
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
