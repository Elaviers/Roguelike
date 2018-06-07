#pragma once
#include "Model.h"
#include "Map.h"
#include "String.h"

class ModelManager
{
private:
	Map<String, Model> _models;

	Model _cube;
	Model _invCube;
	Model _plane;

public:
	ModelManager();
	~ModelManager();

	void Initialise();

	void LoadModel(const char *filepath, const char *name);

	inline Model* GetModel(const char *name) { return _models.Find(name); }

	inline const Model& Cube() const		{ return _cube; }
	inline const Model& InverseCube() const { return _invCube; }
	inline const Model& Plane() const		{ return _plane; }
};
