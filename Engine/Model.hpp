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
	Collider* _collider;

	void _CMD_model(const Buffer<String> &args);
	void _CMD_collision(const Buffer<String> &args);

public:
	//This is stupid. The model is responsible for deletion of these pointers, so don't delete them after using this!
	Model(Mesh *mesh = nullptr, Collider *collider = nullptr) : _mesh(mesh), _collider(collider) {}

	inline void Delete()
	{
		_meshRenderer.Delete();
		delete _collider;
		delete _mesh;
	}

	virtual const PropertyCollection& GetProperties();

	inline GLMeshRenderer& MeshRenderer() { return _meshRenderer; }

	inline const Bounds& GetBounds() const { return _mesh->bounds; }
	inline const Collider* GetCollider() const { return _collider; }
	inline const String& GetDefaultMaterialName() const { return _defaultMaterialName; }

	inline bool operator==(const Model &other) const { return _meshRenderer == other._meshRenderer; }

	inline void Render() const { _meshRenderer.Render(); }
};
