#pragma once
#include "GameObject.h"
#include "Renderer.h"

class Renderable : public GameObject
{
private:
	const Renderer *_renderer;

public:
	Renderable() {}
	virtual ~Renderable() {}

	void SetRenderer(const Renderer& renderer) { _renderer = &renderer; }
	virtual void Render();
};
