#pragma once
#include <ELSys/AssetManager.hpp>
#include "Model.hpp"

class MeshManager;

class ModelManager : public AssetManager<Model>
{
protected:
	virtual Model* _CreateResource(const String&, const String&, const String&, const Context&) override;

	Model _line;
	Model _cube;
	Model _invCube;
	Model _plane;

	SharedPointer<Model> _tLine, _tCube, _tInvCube, _tPlane;

public:
	ModelManager() {}
	virtual ~ModelManager() {}

	void Initialise(MeshManager&);

	const SharedPointer<const Model>& Line() const { return _tLine; }
	const SharedPointer<const Model>& Plane() const { return _tPlane; }
	const SharedPointer<const Model>& Cube() const { return _tCube; }
	const SharedPointer<const Model>& InvCube() const { return _tInvCube; }
};
