#pragma once
#include "Asset.hpp"
#include "Bounds.hpp"
#include "Collider.hpp"
#include "CvarMap.hpp"
#include "GLMeshRenderer.hpp"
#include "Mesh.hpp"
#include "String.hpp"

class Model : public Asset
{
protected:
	CvarMap _cvars;
	
	Mesh* _mesh;
	GLMeshRenderer _meshRenderer;

	String _defaultMaterialName;
	Collider* _collider;

	void _CMD_model(const Buffer<String> &args);
	void _CMD_collision(const Buffer<String> &args);

	virtual void _ReadText(const String &string) override
	{
		Buffer<String> lines = string.ToLower().Split("\r\n");
		for (size_t i = 0; i < lines.GetSize(); ++i)
			String unused = _cvars.HandleCommand(lines[i]);
	}

public:
	//This is stupid. The model is responsible for deletion of these pointers, so don't delete them after using this!
	Model(Mesh *mesh = nullptr, Collider *collider = nullptr) : _mesh(mesh), _collider(collider) {
		_cvars.CreateVar("model", CommandPtr(this, &Model::_CMD_model));
		_cvars.CreateVar("collision", CommandPtr(this, &Model::_CMD_collision));
		_cvars.Add("material", _defaultMaterialName);
	}

	inline void Delete()
	{
		_meshRenderer.Delete();
		delete _collider;
		delete _mesh;
	}

	inline GLMeshRenderer& MeshRenderer() { return _meshRenderer; }

	inline const Bounds& GetBounds() const { return _mesh->bounds; }
	inline const Collider* GetCollider() const { return _collider; }
	inline const String& GetDefaultMaterialName() const { return _defaultMaterialName; }

	inline bool operator==(const Model &other) const { return _meshRenderer == other._meshRenderer; }

	inline void Render() const { _meshRenderer.Render(); }
};
