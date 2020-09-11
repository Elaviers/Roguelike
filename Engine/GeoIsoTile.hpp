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
	static const SharedPointer<const Mesh>& GetMesh() { return _mesh; }
	static void SetMesh(const SharedPointer<const Mesh>& mesh) { _mesh = mesh; }

	GeoIsoTile() {}
	virtual ~GeoIsoTile() {}

	virtual void Render(RenderQueue& q) const override;

	void SetTransform(const Vector3& position, const Vector2& size);

	const SharedPointer<const Material>& GetMaterial() const { return _material; }
	void SetMaterial(const SharedPointer<const Material> material) { _material = material; }
};
