#pragma once
#include "Geometry.hpp"
#include "Tile.hpp"
#include <ELCore/Handle.hpp>
#include <ELCore/SharedPointer.hpp>
#include <ELGraphics/RenderEntry.hpp>
#include <ELMaths/LineSegment.hpp>
#include <ELMaths/Transform.hpp>
#include <ELMaths/Volume.hpp>
#include <ELPhys/Body.hpp>

class GeoIsoTile : public Geometry
{
private:
	RenderEntry _renderEntry;
	void _UpdateRenderEntry();

protected:
	Transform _renderTransform;

	VBox _boundingBox;

	SharedPointer<const Tile> _tile;

	Vector2 _size;

	Handle<FixedBody> _physics;

protected:
	GeoIsoTile(OGeometryCollection& g);
	GeoIsoTile(const GeoIsoTile& other);

public:
	virtual ~GeoIsoTile() {}

	GEOMETRY_VFUNCS(GeoIsoTile, EGeometryID::ISO_TILE);

	GeoIsoTile& operator=(const GeoIsoTile& other)
	{
		_renderTransform = other._renderTransform;
		_boundingBox = other._boundingBox;
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

	virtual void Read(ByteReader&, ObjectIOContext&) override;
	virtual void Write(ByteWriter&, ObjectIOContext&) const override;

	virtual const Volume& GetVolume() const override { return _boundingBox; }
	virtual EOverlapResult Overlaps(const Collider& collider, const Transform& transform, const Vector3& sweep, Vector3* penOut) const 
	{
		LineSegment sweepLine(Vector3(), sweep);
		return _tile->GetCollider().NarrowOverlapCheck(_renderTransform, collider, transform, &sweepLine, penOut);
	}
};
