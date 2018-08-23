#pragma once
#include "GLModel.h"
#include "Map.h"
#include "String.h"

class ModelManager
{
private:
	String _rootPath;

	Map<String, GLModel> _models;

	GLModel _cube;
	GLModel _invCube;
	GLModel _plane;

	GLModel _basicPlane;

public:
	ModelManager();
	~ModelManager();

	void Initialise();

	const GLModel* GetModel(const char *name);

	inline const GLModel& Cube() const			{ return _cube; }
	inline const GLModel& InverseCube() const	{ return _invCube; }
	inline const GLModel& Plane() const			{ return _plane; }

	inline const GLModel& BasicPlane() const	{ return _basicPlane; }

	String FindNameOfModel(const GLModel& model) const;

	inline void SetRootPath(const char *root) { _rootPath = root; }
};
