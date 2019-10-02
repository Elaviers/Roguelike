#include "ObjBrush2D.hpp"
#include "GLProgram.hpp"
#include "MacroUtilities.hpp"
#include "MaterialManager.hpp"
#include "ModelManager.hpp"
#include "Utilities.hpp"

void ObjBrush2D::_OnTransformChanged()
{
	float x = (_point1[0] + _point2[0]) / 2.f;
	float z = (_point1[1] + _point2[1]) / 2.f;
	float w = Maths::Abs(_point1[0] - _point2[0]);
	float h = Maths::Abs(_point1[1] - _point2[1]);

	SetRelativePosition(Vector3(x, level, z));
	SetRelativeScale(Vector3(w, h, 1));
}

void ObjBrush2D::Render(EnumRenderChannel channels) const
{
	if (Engine::Instance().pModelManager && _material && channels & _material->GetRenderChannels())
	{
		_material->Apply();
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, GetTransformationMatrix());
		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(GetRelativeScale()[0], GetRelativeScale()[1]));
		Engine::Instance().pModelManager->Plane().Render();
		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(1.f, 1.f));
	}
}


void ObjBrush2D::WriteData(BufferWriter<byte> &writer, NumberedSet<String> &strings) const
{
	GameObject::WriteData(writer, strings);

	if (Engine::Instance().pMaterialManager && _material)
	{
		uint16 id = strings.Add(Engine::Instance().pMaterialManager->FindNameOf(_material));
		writer.Write_uint16(id);
	}
	else writer.Write_uint16(0);
}

void ObjBrush2D::ReadData(BufferReader<byte> &reader, const NumberedSet<String> &strings)
{
	GameObject::ReadData(reader, strings);

	const String *materialName = strings.Get(reader.Read_uint16());
	if (materialName)
		SetMaterial(*materialName);

	level = GetRelativePosition()[1];
	_point1[0] = GetRelativePosition()[0] - GetRelativeScale()[0] / 2.f;
	_point1[1] = GetRelativePosition()[2] - GetRelativeScale()[1] / 2.f;
	_point2[0] = GetRelativePosition()[0] + GetRelativeScale()[0] / 2.f;
	_point2[1] = GetRelativePosition()[2] + GetRelativeScale()[1] / 2.f;
}

const PropertyCollection& ObjBrush2D::GetProperties()
{
	static PropertyCollection cvars;
	
	DO_ONCE_BEGIN;
	_AddBaseProperties(cvars);

	cvars.Add(
		"Material",
		MemberGetter<ObjBrush<2>, String>(&ObjBrush<2>::GetMaterialName),
		MemberSetter<ObjBrush<2>, String>(&ObjBrush<2>::SetMaterial),
		0,
		PropertyFlags::MATERIAL);

	cvars.Add<Vector3>(
		"Point1",
		offsetof(ObjBrush2D, _point1));

	cvars.Add<Vector3>(
		"Point2",
		offsetof(ObjBrush2D, _point2));

	cvars.Add<float>(
		"Level",
		offsetof(ObjBrush2D, level));
	DO_ONCE_END;

	return cvars;
}
