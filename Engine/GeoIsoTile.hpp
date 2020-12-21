#pragma once
#include "Geometry.hpp"
#include "Tile.hpp"
#include <ELCore/SharedPointer.hpp>
#include <ELGraphics/RenderEntry.hpp>
#include <ELMaths/LineSegment.hpp>
#include <ELMaths/Transform.hpp>

class GeoIsoTile : public Geometry
{
private:
	RenderEntry _renderEntry;
	void _UpdateRenderEntry();

protected:
	Transform _renderTransform;

	Bounds _bounds;

	SharedPointer<const Tile> _tile;

	Vector2 _size;

public:
	//Needed for registry
	static const byte TypeID;

	GeoIsoTile() : _renderEntry(ERenderChannels::UNLIT) { }
	GeoIsoTile(const GeoIsoTile& other) : 
		Geometry(other), 
		_renderEntry(ERenderChannels::UNLIT), 
		_renderTransform(other._renderTransform), 
		_bounds(other._bounds), 
		_tile(other._tile)
	{
		_UpdateRenderEntry();
	}

	virtual ~GeoIsoTile() {}

	GeoIsoTile& operator=(const GeoIsoTile& other)
	{
		_renderTransform = other._renderTransform;
		_bounds = other._bounds;
		_tile = other._tile;
		_UpdateRenderEntry();
		return *this;
	}

	const Vector3& GetPosition() const { return _renderTransform.GetPosition(); }
	void SetPosition(const Vector3& position);
	void SetPositionSize(const Vector3& position, const Vector2& size);

	const SharedPointer<const Tile>& GetTile() const { return _tile; }
	void SetTile(const SharedPointer<const Tile>& tile) { _tile = tile; _UpdateRenderEntry(); }

	virtual void Render(RenderQueue& q) const override;

	virtual void WriteData(ByteWriter&, NumberedSet<String>& strings, const Context& ctx) const override;
	virtual void ReadData(ByteReader&, const NumberedSet<String>& strings, const Context& ctx) override;

	virtual byte GetTypeID() { return TypeID; }

	virtual const Bounds& GetBounds() const { return _bounds; }
	virtual EOverlapResult Overlaps(const Collider& collider, const Transform& transform, const Vector3& sweep, Vector3* penOut) const 
	{
		LineSegment sweepLine(Vector3(), sweep);
		return _tile->GetCollider().Overlaps(_renderTransform, collider, transform, &sweepLine, penOut);
	}
};
