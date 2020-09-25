#pragma once
#include "Geometry.hpp"
#include "Tile.hpp"
#include <ELCore/SharedPointer.hpp>
#include <ELMaths/LineSegment.hpp>
#include <ELMaths/Transform.hpp>

class GeoIsoTile : public Geometry
{
protected:
	Transform _renderTransform;

	Bounds _bounds;

	SharedPointer<const Tile> _tile;

public:
	//Needed for registry
	static const byte TypeID;

	GeoIsoTile() {}
	virtual ~GeoIsoTile() {}

	const Vector3& GetPosition() const { return _renderTransform.GetPosition(); }
	void SetPosition(const Vector3& position);
	void SetTransform(const Vector3& position, const Vector2& size);

	const SharedPointer<const Tile>& GetTile() const { return _tile; }
	void SetTile(const SharedPointer<const Tile>& tile) { _tile = tile; }

	virtual void Render(RenderQueue& q) const override;

	virtual void WriteData(ByteWriter&, NumberedSet<String>& strings, const Context& ctx) const override;
	virtual void ReadData(ByteReader&, const NumberedSet<String>& strings, const Context& ctx) override;

	virtual byte GetTypeID() { return TypeID; }

	virtual const Bounds& GetBounds() const { return _bounds; }
	virtual EOverlapResult Overlaps(const Collider& collider, const Transform& transform, const Vector3& sweep, Vector3* penOut) const 
	{
		return _tile->GetCollider().Overlaps(_renderTransform, collider, transform, &LineSegment(Vector3(), sweep), penOut);
	}
};
