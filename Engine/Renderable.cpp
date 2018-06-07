#include "Renderable.h"

void Renderable::Render()
{
	if (_model)
	{
		GameObject::ApplyTransformToShader();
		_model->Render();
	}
}
