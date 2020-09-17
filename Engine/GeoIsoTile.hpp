#pragma once
#include "Geometry.hpp"
#include "Tile.hpp"
#include <ELCore/SharedPointer.hpp>
#include <ELGraphics/Material.hpp>
#include <ELMaths/Transform.hpp>

class GeoIsoTile : public Geometry
{
protected:
	Transform _renderTransform;

	SharedPointer<const Tile> _tile;

	//todo: Perhaps this shouldn't be static.. but it is for now!
	static SharedPointer<const Mesh> _mesh;

public:
	//Needed for registry
	static const byte TypeID;

	static const SharedPointer<const Mesh>& GetMesh() { return _mesh; }
	static void SetMesh(const SharedPointer<const Mesh>& mesh) { _mesh = mesh; }

	GeoIsoTile() {}
	virtual ~GeoIsoTile() {}

	void SetTransform(const Vector3& position, const Vector2& size);

	const SharedPointer<const Tile>& GetTile() const { return _tile; }
	void SetTile(const SharedPointer<const Tile>& tile) { _tile = tile; }

	virtual void Render(RenderQueue& q) const override;

	virtual void WriteData(ByteWriter&, NumberedSet<String>& strings, const Context& ctx) const override;
	virtual void ReadData(ByteReader&, const NumberedSet<String>& strings, const Context& ctx) override;

	virtual byte GetTypeID() { return TypeID; }
};
