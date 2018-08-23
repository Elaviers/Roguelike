#include "Renderable.h"

MaterialManager* Renderable::_materialManager;
ModelManager* Renderable::_modelManager;

void Renderable::GetProperties(ObjectProperties &properties)
{
	_AddBaseProperties(properties);
	properties.Add<Renderable, String>("Model", this, &Renderable::GetModelName, &Renderable::SetModel);
	properties.Add<Renderable, String>("Material", this, &Renderable::GetMaterialName, &Renderable::SetMaterial);
}

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

String Renderable::GetModelName() const
{
	return _modelManager->FindNameOfModel(*_model);
}

String Renderable::GetMaterialName() const
{
	return _materialManager->FindNameOfMaterial(*_material);
}
