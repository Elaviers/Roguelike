#pragma once
#include "GLModel.h"
#include "Map.h"
#include "String.h"

class ModelManager
{
private:
	Map<String, GLModel> _models;

	GLModel _cube;
	GLModel _invCube;
	GLModel _plane;

	GLModel _basicPlane;

public:
	ModelManager();
	~ModelManager();

	void Initialise();

	void LoadModel(const char *filepath, const char *name);

	inline GLModel* GetModel(const char *name) { return _models.Find(name); }

	inline const GLModel& Cube() const		{ return _cube; }
	inline const GLModel& InverseCube() const { return _invCube; }
	inline const GLModel& Plane() const		{ return _plane; }

	inline const GLModel& BasicPlane() const	{ return _basicPlane; }
};
