#pragma once
#include "Geometry.hpp"
#include <ELCore/SharedPointer.hpp>
#include <ELGraphics/Material.hpp>
#include <ELMaths/Transform.hpp>

class GeoIsoTile : public Geometry
{
protected:
	Transform _renderTransform;

	SharedPointer<const Material> _material;

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

	const SharedPointer<const Material>& GetMaterial() const { return _material; }
	void SetMaterial(const SharedPointer<const Material> material) { _material = material; }

	virtual void Render(RenderQueue& q) const override;

	virtual void WriteData(ByteWriter&, NumberedSet<String>& strings, const Context& ctx) const override;
	virtual void ReadData(ByteReader&, const NumberedSet<String>& strings, const Context& ctx) override;

	virtual byte GetTypeID() { return TypeID; }
};
