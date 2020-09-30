#include "GeoIsoTile.hpp"
#include "EGeometryID.hpp"
#include "TileManager.hpp"
#include <ELGraphics/RenderQueue.hpp>

const byte GeoIsoTile::TypeID = (byte)EGeometryID::ISO_TILE;

void GeoIsoTile::_UpdateRenderEntry()
{
	_renderEntry.Clear();

	if (_tile && _tile->GetMesh() && _tile->GetMaterial())
	{
		_renderEntry.AddSetTransform(_renderTransform.GetTransformationMatrix());
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

void GeoIsoTile::WriteData(ByteWriter& data, NumberedSet<String>& strings, const Context& ctx) const
{
	data.Write<Vector3>(_renderTransform.GetPosition());
	data.Write<Vector2>(_size);

	data.Write_uint16(_tile ? strings.Add(ctx.GetPtr<TileManager>()->FindNameOf(_tile.Ptr())) : 0);
}

void GeoIsoTile::ReadData(ByteReader& data, const NumberedSet<String>& strings, const Context& ctx)
{
	Vector3 pos = data.Read<Vector3>();
	Vector2 size = data.Read<Vector2>();
	_bounds = Bounds(_renderTransform.GetScale() / 2.f, _renderTransform.GetPosition() + _renderTransform.GetScale() / 2.f);

	const String* tname = strings.Get(data.Read_uint16());
	if (tname)
		_tile = ctx.GetPtr<TileManager>()->Get(*tname, ctx);

	SetPositionSize(pos, size);
}

void GeoIsoTile::SetPosition(const Vector3& position)
{
	_renderTransform.SetPosition(position);
	_bounds = Bounds(_renderTransform.GetScale() / 2.f, _renderTransform.GetPosition() + _renderTransform.GetScale() / 2.f);
	_UpdateRenderEntry();
}

void GeoIsoTile::SetPositionSize(const Vector3& position, const Vector2& size)
{
	Vector2 finalSize = _size = size;

	if (_tile)
		finalSize *= _tile->GetRenderSize();

	_renderTransform.SetPosition(position);
	_renderTransform.SetScale(Vector3(finalSize.x, finalSize.y, finalSize.x));
	_bounds = Bounds(_renderTransform.GetScale() / 2.f, _renderTransform.GetPosition() + _renderTransform.GetScale() / 2.f);
	_UpdateRenderEntry();
}
