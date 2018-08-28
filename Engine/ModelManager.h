#pragma once
#include "ResourceManagerBase.h"
#include "Model.h"

class ModelManager : public ResourceManagerBase<Model>
{
private:
	Model _cube;
	Model _invCube;
	Model _plane;

	Model _basicPlane;

public:
	ModelManager();
	~ModelManager();

	void Initialise();

	const Model* GetModel(const String &name);

	inline const Model& Cube() const			{ return _cube; }
	inline const Model& InverseCube() const	{ return _invCube; }
	inline const Model& Plane() const			{ return _plane; }

	inline const Model& BasicPlane() const	{ return _basicPlane; }
};
