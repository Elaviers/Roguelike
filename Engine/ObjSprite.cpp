#include "ObjSprite.hpp"
#include "GLProgram.hpp"
#include "MacroUtilities.hpp"
#include "ModelManager.hpp"
#include "ObjCamera.hpp"

void ObjSprite::Render(EnumRenderChannel channels) const
{
	if (Engine::Instance().pModelManager && ObjCamera::Current() && _material && channels & _material->GetRenderChannels())
	{
		Mat4 t;

		if (_lookAtCamera)
		{
			t = Matrix::Transformation(GetWorldPosition(), ObjCamera::Current()->GetWorldRotation().GetQuat(), Vector3(_size, _size, _size));
		}
		else
		{
			Vector3 pos = GetWorldPosition();
			Vector3 delta = pos - ObjCamera::Current()->GetWorldPosition();
			delta.Normalise();

			float y = Maths::ArcTangentDegrees2(delta[0], delta[2]);
			Vector3 e = Vector3(0, y, 0);
			t = Matrix::Transformation(pos, e, Vector3(_size, _size, _size));
		}
		
		_material->Apply();

		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t);
		GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, _colour);

		Engine::Instance().pModelManager->Plane().Render();
	}
}

const PropertyCollection& ObjSprite::GetProperties()
{
	static PropertyCollection cvars;

	DO_ONCE_BEGIN;
	_AddBaseProperties(cvars);

	cvars.Add(
		"Material",
		MemberGetter<ObjSprite, String>(&ObjSprite::GetMaterialName),
		MemberSetter<ObjSprite, String>(&ObjSprite::SetMaterialName),
		0,
		PropertyFlags::MATERIAL);

	cvars.Add<float>(
		"Size",
		offsetof(ObjSprite, _size));

	cvars.Add<bool>(
		"Look at camera",
		offsetof(ObjSprite, _lookAtCamera));
	DO_ONCE_END;

	return cvars;
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
