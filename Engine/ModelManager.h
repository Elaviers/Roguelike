#pragma once
#include "ResourceManagerBase.h"
#include "Model.h"

class ModelManager : public ResourceManagerBase<Model>
{
private:
	Model _line;
	Model _cube;
	Model _invCube;
	Model _plane;

public:
	ModelManager();
	~ModelManager();

	void Initialise();

	const Model* GetModel(const String &name);

	inline const Model& Line() const			{ return _line; }
	inline const Model& Plane() const			{ return _plane; }
	inline const Model& Cube() const			{ return _cube; }
	inline const Model& InverseCube() const		{ return _invCube; }
};
