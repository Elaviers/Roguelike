#include "ObjRenderable.h"
#include "Collider.h"
#include "CvarMap.h"
#include "GLProgram.h"

void ObjRenderable::GetCvars(CvarMap &cvars)
{
	_AddBaseCvars(cvars);
	cvars.Add("Model", Getter<String>(this, &ObjRenderable::GetModelName), Setter<String>(this, &ObjRenderable::SetModel), PropertyFlags::MODEL);
	cvars.Add("Material", Getter<String>(this, &ObjRenderable::GetMaterialName), Setter<String>(this, &ObjRenderable::SetMaterial), PropertyFlags::MATERIAL);
}

void ObjRenderable::Render() const
{
	if (_model)
	{
		if (_material)
		{
			if (! (GLProgram::Current().GetChannels() & _material->GetShaderChannels()))
				return;

			_material->Apply();
		}

		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, GetTransformationMatrix());
		GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, Vector4(1.f, 1.f, 1.f, 1.f));
		_model->model.Render();
	}
}

const Collider* ObjRenderable::GetCollider() const
{
	if (_model)
	{
		Collider* c = _model->collider;
		
		return _model->collider;
	}

	return nullptr;
}

String ObjRenderable::GetModelName() const
{
	if (Engine::Instance().pModelManager && _model) return Engine::Instance().pModelManager->FindNameOf(*_model);
	return "Unknown";
}

String ObjRenderable::GetMaterialName() const
{																						//todo: remove Smelly const cast
	if (Engine::Instance().pMaterialManager && _material) return Engine::Instance().pMaterialManager->FindNameOf(const_cast<Material*>(_material));
	return "Unknown";
}

void ObjRenderable::WriteToFile(BufferIterator<byte> &buffer, NumberedSet<String> &strings) const
{
	if (Engine::Instance().pModelManager && _model)
	{
		uint16 id = strings.Add(Engine::Instance().pModelManager->FindNameOf(*_model));
		buffer.Write_uint16(id);
	}
	else buffer.Write_uint16(0);

	if (!_materialIsDefault && Engine::Instance().pMaterialManager && _material)
	{																	//todo: const cast removal
		uint16 id = strings.Add(Engine::Instance().pMaterialManager->FindNameOf(const_cast<Material*>(_material)));
		buffer.Write_uint16(id);
	}
	else buffer.Write_uint16(0);

	RelativeTransform().WriteToBuffer(buffer);
}

void ObjRenderable::ReadFromFile(BufferIterator<byte> &buffer, const NumberedSet<String> &strings)
{
	const String *string;

	if (string = strings.Find(buffer.Read_uint16()))
		SetModel(*string);

	if (string = strings.Find(buffer.Read_uint16()))
		SetMaterial(*string);

	RelativeTransform().ReadFromBuffer(buffer);
}
