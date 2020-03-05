#include "EntBrush2D.hpp"
#include "GLProgram.hpp"
#include "MacroUtilities.hpp"
#include "MaterialManager.hpp"
#include "ModelManager.hpp"
#include "Utilities.hpp"

void EntBrush2D::_OnTransformChanged()
{
	if (!_updatingTransform)
	{
		float hw = GetRelativeScale()[0] / 2.f;
		float hh = GetRelativeScale()[1] / 2.f;

		_point1 = Vector2(GetRelativePosition()[0] - hw, GetRelativePosition()[2] - hh);
		_point2 = Vector2(GetRelativePosition()[0] + hh, GetRelativePosition()[2] + hh);
	}
}

void EntBrush2D::_OnPointChanged()
{
	float x = (_point1[0] + _point2[0]) / 2.f;
	float z = (_point1[1] + _point2[1]) / 2.f;
	float w = Maths::Abs(_point1[0] - _point2[0]);
	float h = Maths::Abs(_point1[1] - _point2[1]);

	_updatingTransform = true;
	SetRelativePosition(Vector3(x, level, z));
	SetRelativeScale(Vector3(w, h, 0.f));
	_updatingTransform = false;
}

void EntBrush2D::Render(ERenderChannels channels) const
{
	if (Engine::Instance().pModelManager && _material && channels & _material->GetRenderChannelss())
	{
		_material->Apply();
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, GetTransformationMatrix());
		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(GetRelativeScale()[0], GetRelativeScale()[1]));
		Engine::Instance().pModelManager->Plane()->Render();
		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(1.f, 1.f));
	}
}


void EntBrush2D::WriteData(BufferWriter<byte> &writer, NumberedSet<String> &strings) const
{
	Entity::WriteData(writer, strings);

	if (Engine::Instance().pMaterialManager && _material)
	{
		uint16 id = strings.Add(Engine::Instance().pMaterialManager->FindNameOf(_material.Ptr()));
		writer.Write_uint16(id);
	}
	else writer.Write_uint16(0);
}

void EntBrush2D::ReadData(BufferReader<byte> &reader, const NumberedSet<String> &strings)
{
	Entity::ReadData(reader, strings);

	const String *materialName = strings.Get(reader.Read_uint16());
	if (materialName)
		SetMaterial(*materialName);

	level = GetRelativePosition()[1];
	_point1[0] = GetRelativePosition()[0] - GetRelativeScale()[0] / 2.f;
	_point1[1] = GetRelativePosition()[2] - GetRelativeScale()[1] / 2.f;
	_point2[0] = GetRelativePosition()[0] + GetRelativeScale()[0] / 2.f;
	_point2[1] = GetRelativePosition()[2] + GetRelativeScale()[1] / 2.f;
}

const PropertyCollection& EntBrush2D::GetProperties()
{
	static PropertyCollection cvars;
	
	DO_ONCE_BEGIN;
	_AddBaseProperties(cvars);

	cvars.Add(
		"Material",
		MemberGetter<EntBrush2D, String>(&EntBrush2D::GetMaterialName),
		MemberSetter<EntBrush2D, String>(&EntBrush2D::SetMaterial),
		0,
		PropertyFlags::MATERIAL);

	cvars.Add(
		"Point1",
		MemberGetter<EntBrush2D, const Vector2&>(&EntBrush2D::GetPoint1),
		MemberSetter<EntBrush2D, Vector2>(&EntBrush2D::SetPoint1));

	cvars.Add(
		"Point2",
		MemberGetter<EntBrush2D, const Vector2&>(&EntBrush2D::GetPoint2),
		MemberSetter<EntBrush2D, Vector2>(&EntBrush2D::SetPoint2));

	cvars.Add<float>(
		"Level",
		offsetof(EntBrush2D, level));
	DO_ONCE_END;

	return cvars;
}
