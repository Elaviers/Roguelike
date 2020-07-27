#pragma once
#include <ELCore/SharedPointer.hpp>
#include <ELGraphics/Mesh.hpp>
#include <ELPhys/Collider.hpp>

class Model : public Asset
{
protected:
	SharedPointer<const Mesh> _mesh;

	String _defaultMaterialName;
	Collider _collider;

	void _CMD_mesh(const Buffer<String>& args, const Context&);
	void _CMD_collision(const Buffer<String>& args, const Context&);

public:
	Model() {}
	virtual ~Model() {}

	virtual const PropertyCollection& GetProperties();

	const SharedPointer<const Mesh>& GetMesh() const { return _mesh; }
	const Collider& GetCollider() const { return _collider; }
	const String& GetDefaultMaterialName() const { return _defaultMaterialName; }

	void SetMesh(const SharedPointer<const Mesh>& mesh) { _mesh = mesh; }
	void SetCollider(const Collider& collider) { _collider = collider; }
	void SetDefaultMaterialName(const String& defaultMaterial) { _defaultMaterialName = defaultMaterial; }
};

