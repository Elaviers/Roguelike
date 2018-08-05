#include "Renderable.h"

void Renderable::Render()
{
	if (_model)
	{
		GameObject::ApplyTransformToShader();
		
		if (_material)
			_material->Apply();
		
		_model->Render();
	}
}
