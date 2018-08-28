#include "Renderable.h"

MaterialManager* Renderable::_materialManager;
ModelManager* Renderable::_modelManager;

void Renderable::GetProperties(ObjectProperties &properties)
{
	_AddBaseProperties(properties);
	properties.Add<Renderable, String>("Model", this, &Renderable::GetModelName, &Renderable::SetModel, PropertyFlags::MODEL);
	properties.Add<Renderable, String>("Material", this, &Renderable::GetMaterialName, &Renderable::SetMaterial, PropertyFlags::MATERIAL);
}

void Renderable::Render() const
{
	if (_model)
	{
		GameObject::ApplyTransformToShader();
		
		if (_material)
			_material->Apply();
		
		_model->model.Render();
	}
}

String Renderable::GetModelName() const
{
	return _modelManager->FindNameOf(*_model);
}

String Renderable::GetMaterialName() const
{
	if (_material) return _materialManager->FindNameOf(*_material);
	return "Unknown";
}
