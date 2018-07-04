#pragma once
#include "GameObject.h"
#include "GLModel.h"

class Renderable : public GameObject
{
private:
	const GLModel *_model;

public:
	Renderable() {}
	virtual ~Renderable() {}

	void SetModel(const GLModel& renderer) { _model = &renderer; }
	virtual void Render();
};
