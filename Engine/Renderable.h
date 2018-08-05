#pragma once
#include "GameObject.h"
#include "GLModel.h"
#include "Material.h"

class Renderable : public GameObject
{
private:
	const GLModel *_model;
	const Material *_material;

public:
	Renderable() {}
	virtual ~Renderable() {}

	void SetModel(const GLModel* renderer) { _model = renderer; }
	void SetMaterial(const Material* material) { _material = material; }
	virtual void Render();
};
