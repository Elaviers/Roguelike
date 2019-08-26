#include "ObjSprite.hpp"
#include "GLProgram.hpp"
#include "ModelManager.hpp"
#include "ObjCamera.hpp"

void ObjSprite::Render(EnumRenderChannel channels) const
{
	if (Engine::Instance().pModelManager && ObjCamera::Current() && _material && channels & _material->GetRenderChannels())
	{
		Vector3 position = Vector3() * GetTransformationMatrix();

		Mat4 t = Matrix::Scale(Vector3(_size, _size, _size)) * ObjCamera::Current()->GetWorldTransform().GetRotation().GetQuat().ToMatrix() * Matrix::Translation(position);

		_material->Apply();

		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t);
		GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, _colour);

		Engine::Instance().pModelManager->Plane().Render();
	}
}

void ObjSprite::GetCvars(CvarMap& cvars)
{
	_AddBaseCvars(cvars);

	cvars.Add("Material", Getter<String>(this, &ObjSprite::GetMaterialName), Setter<String>(this, &ObjSprite::SetMaterialName), CvarFlags::MATERIAL);
	cvars.Add("Size", _size);
}

void ObjSprite::WriteData(BufferWriter<byte>& writer, NumberedSet<String>& strings) const
{
	GameObject::WriteData(writer, strings);

	writer.Write_float(_size);
	writer.Write_vector4(_colour);

	if (Engine::Instance().pMaterialManager && _material)
	{
		uint16 id = strings.Add(Engine::Instance().pMaterialManager->FindNameOf(_material));
		writer.Write_uint16(id);
	}
	else writer.Write_uint16(0);
}

void ObjSprite::ReadData(BufferReader<byte>& reader, const NumberedSet<String>& strings)
{
	GameObject::ReadData(reader, strings);

	_size = reader.Read_float();
	_colour = reader.Read_vector4();

	const String* materialName = strings.Get(reader.Read_uint16());

	if (Engine::Instance().pMaterialManager && materialName)
		_material = (MaterialSprite*)Engine::Instance().pMaterialManager->Get(*materialName);
}
