#include "ObjRenderable.hpp"
#include "Collider.hpp"
#include "CvarMap.hpp"
#include "GLProgram.hpp"

void ObjRenderable::GetCvars(CvarMap &cvars)
{
	_AddBaseCvars(cvars);
	cvars.Add("Model", Getter<String>(this, &ObjRenderable::GetModelName), Setter<String>(this, &ObjRenderable::SetModel), CvarFlags::MODEL);
	cvars.Add("Material", Getter<String>(this, &ObjRenderable::GetMaterialName), Setter<String>(this, &ObjRenderable::SetMaterial), CvarFlags::MATERIAL);
}

void ObjRenderable::Render(EnumRenderChannel channels) const
{
	if (_model)
	{
		if (_material)
		{
			if (! (channels & _material->GetRenderChannels()))
				return;

			_material->Apply();
		}

		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, GetTransformationMatrix());
		GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, _colour);
		_model->Render();
	}
}

const Collider* ObjRenderable::GetCollider() const
{
	if (_model)
		return _model->GetCollider();

	return nullptr;
}

String ObjRenderable::GetModelName() const
{
	if (Engine::Instance().pModelManager && _model) return Engine::Instance().pModelManager->FindNameOf(_model);
	return "Unknown";
}

String ObjRenderable::GetMaterialName() const
{																						//todo: remove Smelly const cast
	if (Engine::Instance().pMaterialManager && _material) return Engine::Instance().pMaterialManager->FindNameOf(const_cast<Material*>(_material));
	return "Unknown";
}

void ObjRenderable::WriteData(BufferWriter<byte> &writer, NumberedSet<String> &strings) const
{
	GameObject::WriteData(writer, strings);

	if (Engine::Instance().pModelManager && _model)
	{
		uint16 id = strings.Add(Engine::Instance().pModelManager->FindNameOf(_model));
		writer.Write_uint16(id);
	}
	else writer.Write_uint16(0);

	if (!_materialIsDefault && Engine::Instance().pMaterialManager && _material)
	{																	//todo: const cast removal
		uint16 id = strings.Add(Engine::Instance().pMaterialManager->FindNameOf(const_cast<Material*>(_material)));
		writer.Write_uint16(id);
	}
	else writer.Write_uint16(0);

	writer.Write_vector4(_colour);
}

void ObjRenderable::ReadData(BufferReader<byte> &reader, const NumberedSet<String> &strings)
{
	GameObject::ReadData(reader, strings);

	const String *string;

	if (string = strings.Get(reader.Read_uint16()))
		SetModel(*string);

	if (string = strings.Get(reader.Read_uint16()))
		SetMaterial(*string);

	_colour = reader.Read_vector4();
}
