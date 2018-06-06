#include "Renderable.h"

void Renderable::Render()
{
	if (_renderer)
	{
		GameObject::ApplyTransformToShader();
		_renderer->Render();
	}
}
