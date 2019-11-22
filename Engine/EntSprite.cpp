#include "EntSprite.hpp"
#include "GLProgram.hpp"
#include "MacroUtilities.hpp"
#include "ModelManager.hpp"
#include "EntCamera.hpp"

void EntSprite::Render(EnumRenderChannel channels) const
{
	if (Engine::Instance().pModelManager && EntCamera::Current() && _material && channels & _material->GetRenderChannels())
	{
		Mat4 t;

		if (_fixedYaw)
		{
			Vector3 pos = GetWorldPosition();
			Vector3 delta = pos - EntCamera::Current()->GetWorldPosition();
			delta.Normalise();

			float y = Maths::ArcTangentDegrees2(delta[0], delta[2]);
			Vector3 e = Vector3(0, y, 0);
			t = Matrix::Transformation(pos, e, Vector3(_size, _size, _size));
		}
		else
			t = Matrix::Transformation(GetWorldPosition(), EntCamera::Current()->GetWorldRotation().GetQuat(), Vector3(_size, _size, _size));
		
		_material->Apply();

		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t);
		GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, _colour);

		Engine::Instance().pModelManager->Plane()->Render();
	}
}

const PropertyCollection& EntSprite::GetProperties()
{
	static PropertyCollection cvars;

	DO_ONCE_BEGIN;
	_AddBaseProperties(cvars);

	cvars.Add(
		"Material",
		MemberGetter<EntSprite, String>(&EntSprite::GetMaterialName),
		MemberSetter<EntSprite, String>(&EntSprite::SetMaterialName),
		0,
		PropertyFlags::MATERIAL);

	cvars.Add<float>(
		"Size",
		offsetof(EntSprite, _size));

	cvars.Add<bool>(
		"FixedYaw",
		offsetof(EntSprite, _fixedYaw));
	DO_ONCE_END;

	return cvars;
}

void EntSprite::WriteData(BufferWriter<byte>& writer, NumberedSet<String>& strings) const
{
	Entity::WriteData(writer, strings);

	writer.Write_float(_size);
	writer.Write_vector4(_colour);

	if (Engine::Instance().pMaterialManager && _material)
	{
		uint16 id = strings.Add(Engine::Instance().pMaterialManager->FindNameOf(_material.Ptr()));
		writer.Write_uint16(id);
	}
	else writer.Write_uint16(0);
}

void EntSprite::ReadData(BufferReader<byte>& reader, const NumberedSet<String>& strings)
{
	Entity::ReadData(reader, strings);

	_size = reader.Read_float();
	_colour = reader.Read_vector4();

	const String* materialName = strings.Get(reader.Read_uint16());

	if (Engine::Instance().pMaterialManager && materialName)
		_material = Engine::Instance().pMaterialManager->Get(*materialName).Cast<const MaterialSprite>();
}
