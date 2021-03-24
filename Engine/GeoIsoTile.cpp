#include "GeoIsoTile.hpp"
#include "EGeometryID.hpp"
#include "TileManager.hpp"
#include "World.hpp"
#include <ELGraphics/RenderQueue.hpp>

GeoIsoTile::GeoIsoTile(OGeometryCollection& g) :
	Geometry(g),
	_renderEntry(ERenderChannels::UNLIT),
	_physics(g.GetWorld().CreateFixedBody(GetOwner()))
{
}

GeoIsoTile::GeoIsoTile(const GeoIsoTile& other) :
	Geometry(other),
	_renderEntry(ERenderChannels::UNLIT),
	_renderTransform(other._renderTransform),
	_boundingBox(other._boundingBox),
	_tile(other._tile),
	_physics(other.GetOwner()->GetWorld().CreateFixedBody(GetOwner()))
{
	_UpdateRenderEntry();
	_physics->Collision() = other._physics->Collision();
	_physics->SetTransform(other._physics->GetTransform());
}

void GeoIsoTile::_UpdateRenderEntry()
{
	_renderEntry.Clear();

	//physics updated here as well!!
	_physics->SetTransform(_renderTransform);

	if (_tile && _tile->GetMesh() && _tile->GetMaterial())
	{
		//oh, and the bounding box!!
		if (const Volume* v = _tile->GetMesh()->GetVolume())
		{
			Vector3 min, max;
			v->Bounds_AABB(_renderTransform, min, max);
			_boundingBox.SetTransform(Transform((min + max) / 2.f, Rotation(), max - min));
		}
		else
		{
			Debug::PrintLine("GeoIsoTile Warning - mesh does not have a volume! Using transformed identity..");

			_boundingBox.SetTransform(_renderTransform);
		}

		_renderEntry.AddSetTransform(_renderTransform.GetMatrix());
		_renderEntry.AddSetColour();
		_tile->GetMaterial()->Apply(_renderEntry);
		_renderEntry.AddSetUVOffset();
		_renderEntry.AddSetUVScale();
		_renderEntry.AddRenderMesh(*_tile->GetMesh());
	}
}

void GeoIsoTile::Render(RenderQueue& q) const
{
	q.AddEntry(&_renderEntry);
}

void GeoIsoTile::Write(ByteWriter& data, ObjectIOContext& ctx) const
{
	data.Write<Vector3>(_renderTransform.GetPosition());
	data.Write<Vector2>(_size);

	data.Write_uint16(_tile ? ctx.strings.Add(ctx.context.GetPtr<TileManager>()->FindNameOf(_tile.Ptr())) : 0);
}

void GeoIsoTile::Read(ByteReader& data, ObjectIOContext& ctx)
{
	Vector3 pos = data.Read<Vector3>();
	Vector2 size = data.Read<Vector2>();
	
	const String* tname = ctx.strings.Get(data.Read_uint16());
	if (tname)
		_tile = ctx.context.GetPtr<TileManager>()->Get(*tname, ctx.context);

	SetPositionSize(pos, size);
}

void GeoIsoTile::SetPosition(const Vector3& position)
{
	_renderTransform.SetPosition(position);
	_boundingBox.SetPosition(_renderTransform.GetPosition() + _renderTransform.GetScale());
	_UpdateRenderEntry();
}

void GeoIsoTile::SetPositionSize(const Vector3& position, const Vector2& size)
{
	Vector2 finalSize = _size = size;

	if (_tile)
		finalSize *= _tile->GetRenderSize();

	_renderTransform.SetPosition(position);
	_renderTransform.SetScale(Vector3(finalSize.x, finalSize.y, finalSize.x));
	
	_boundingBox.SetScale(_renderTransform.GetScale());
	_UpdateRenderEntry();
}
