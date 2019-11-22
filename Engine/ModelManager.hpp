#pragma once
#include "AssetManager.hpp"
#include "Model.hpp"

class ModelManager : public AssetManager<Model>
{
private:
	Model *_line = nullptr;
	Model *_cube = nullptr;
	Model *_invCube = nullptr;
	Model *_plane = nullptr;

	SharedPointer<const Model> _tLine, _tCube, _tInvCube, _tPlane;

	virtual Model* _CreateResource(const String&, const String&) override;

	virtual void _DestroyResource(Model&);

public:
	ModelManager();
	virtual ~ModelManager();

	void Initialise();

	const SharedPointer<const Model>& Line() const { return _tLine; }
	const SharedPointer<const Model>& Plane() const { return _tPlane; }
	const SharedPointer<const Model>& Cube() const { return _tCube; }
	const SharedPointer<const Model>& InverseCube() const { return _tInvCube; }
};
