#pragma once
#include "GameObject.h"
#include "Model.h"

class Renderable : public GameObject
{
private:
	const Model *_model;

public:
	Renderable() {}
	virtual ~Renderable() {}

	void SetModel(const Model& renderer) { _model = &renderer; }
	virtual void Render();
};
