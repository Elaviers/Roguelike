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

	virtual Model* _CreateResource(const String&, const String&) override;

	virtual void _DestroyResource(Model&);

public:
	ModelManager();
	virtual ~ModelManager();

	void Initialise();

	inline const Model& Line() const			{ return *_line; }
	inline const Model& Plane() const			{ return *_plane; }
	inline const Model& Cube() const			{ return *_cube; }
	inline const Model& InverseCube() const		{ return *_invCube; }
};
