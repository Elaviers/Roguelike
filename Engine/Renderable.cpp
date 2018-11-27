#include "Renderable.h"
#include "ObjectProperties.h"

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
		if (_material)
			_material->Apply();

		GameObject::ApplyTransformToShader();
		_model->model.Render();
	}
}

const Collider* Renderable::GetCollider() const
{
	if (_model)
		return _model->collider;

	return nullptr;
}

String Renderable::GetModelName() const
{
	if (Engine::modelManager && _model) return Engine::modelManager->FindNameOf(*_model);
	return "Unknown";
}

String Renderable::GetMaterialName() const
{																						//todo: remove Smelly const cast
	if (Engine::materialManager && _material) return Engine::materialManager->FindNameOf(const_cast<Material*>(_material));
	return "Unknown";
}

void Renderable::SaveToFile(BufferIterator<byte> &buffer, const Map<String, uint16> &strings) const
{
	buffer.Write_byte(Engine::ObjectIDs::RENDERABLE);

	if (Engine::modelManager && _model)
	{
		const uint16 *id = strings.Find(Engine::modelManager->FindNameOf(*_model));
		if (id) buffer.Write_uint16(*id);
		else buffer.Write_uint16(0);
	}
	else buffer.Write_uint16(0);

	if (!_materialIsDefault && Engine::materialManager && _material)
	{																	//todo: const cast removal
		const uint16 *id = strings.Find(Engine::materialManager->FindNameOf(const_cast<Material*>(_material)));
		if (id) buffer.Write_uint16(*id);
		else buffer.Write_uint16(0);
	}
	else buffer.Write_uint16(0);

	transform.WriteToBuffer(buffer);
}

void Renderable::LoadFromFile(BufferIterator<byte> &buffer, const Map<uint16, String> &strings)
{
	const String *string;

	if (string = strings.Find(buffer.Read_uint16()))
		SetModel(*string);

	if (string = strings.Find(buffer.Read_uint16()))
		SetMaterial(*string);

	transform.ReadFromBuffer(buffer);
}
