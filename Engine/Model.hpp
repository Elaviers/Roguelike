#pragma once
#include "Asset.hpp"
#include "Bounds.hpp"
#include "Collider.hpp"
#include "GLMeshRenderer.hpp"
#include "Mesh.hpp"
#include "String.hpp"

class Model : public Asset
{
protected:
	Mesh* _mesh;
	GLMeshRenderer _meshRenderer;

	String _defaultMaterialName;
	Collider _collider;

	void _CMD_model(const Buffer<String> &args);
	void _CMD_collision(const Buffer<String> &args);

public:
	//This is stupid. The model is responsible for deletion of the mesh, so don't delete them it using this!
	Model(Mesh *mesh = nullptr, const Collider& collider = Collider(CollisionChannels::ALL)) : _mesh(mesh), _collider(collider) {}

	void Delete()
	{
		_meshRenderer.Delete();
		delete _mesh;
	}

	virtual const PropertyCollection& GetProperties();

	GLMeshRenderer& MeshRenderer() { return _meshRenderer; }

	const Mesh* GetMesh() const { return _mesh; }
	const Bounds& GetBounds() const { return _mesh->bounds; }
	const Collider& GetCollider() const { return _collider; }
	const String& GetDefaultMaterialName() const { return _defaultMaterialName; }

	bool operator==(const Model &other) const { return _meshRenderer == other._meshRenderer; }

	void Render() const { _meshRenderer.Render(); }
};
