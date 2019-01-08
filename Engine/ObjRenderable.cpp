#include "ObjRenderable.h"
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
		_model->model.Render();
	}
}

const Collider* ObjRenderable::GetCollider() const
{
	if (_model)
		return _model->collider;

	return nullptr;
}

String ObjRenderable::GetModelName() const
{
	if (Engine::modelManager && _model) return Engine::modelManager->FindNameOf(*_model);
	return "Unknown";
}

String ObjRenderable::GetMaterialName() const
{																						//todo: remove Smelly const cast
	if (Engine::materialManager && _material) return Engine::materialManager->FindNameOf(const_cast<Material*>(_material));
	return "Unknown";
}

void ObjRenderable::WriteToFile(BufferIterator<byte> &buffer, NumberedSet<String> &strings) const
{
	if (Engine::modelManager && _model)
	{
		uint16 id = strings.Add(Engine::modelManager->FindNameOf(*_model));
		buffer.Write_uint16(id);
	}
	else buffer.Write_uint16(0);

	if (!_materialIsDefault && Engine::materialManager && _material)
	{																	//todo: const cast removal
		uint16 id = strings.Add(Engine::materialManager->FindNameOf(const_cast<Material*>(_material)));
		buffer.Write_uint16(id);
	}
	else buffer.Write_uint16(0);

	transform.WriteToBuffer(buffer);
}

void ObjRenderable::ReadFromFile(BufferIterator<byte> &buffer, const NumberedSet<String> &strings)
{
	const String *string;

	if (string = strings.Find(buffer.Read_uint16()))
		SetModel(*string);

	if (string = strings.Find(buffer.Read_uint16()))
		SetMaterial(*string);

	transform.ReadFromBuffer(buffer);
}
