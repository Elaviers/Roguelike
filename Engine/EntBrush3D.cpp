#include "EntBrush3D.hpp"
#include "GLProgram.hpp"
#include "MacroUtilities.hpp"
#include "Utilities.hpp"

void EntBrush3D::_OnTransformChanged()
{
	float x = (_point1[0] + _point2[0]) / 2.f;
	float y = (_point1[1] + _point2[1]) / 2.f;
	float z = (_point1[2] + _point2[2]) / 2.f;
	float w = Maths::Abs(_point1[0] - _point2[0]);
	float h = Maths::Abs(_point1[1] - _point2[1]);
	float d = Maths::Abs(_point1[2] - _point2[2]);

	SetRelativePosition(Vector3(x, y, z));
	SetRelativeScale(Vector3(w, h, d));
}

#include "DrawUtils.hpp"

void EntBrush3D::Render(EnumRenderChannel channels) const
{
	ModelManager* modelManager = Engine::Instance().pModelManager;

	if (modelManager && _material && channels & _material->GetRenderChannels())
	{
		_material->Apply();

		Mat4 pt = _parent ? _parent->GetTransformationMatrix() : Mat4();

		Transform t;

		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(GetRelativeScale()[0], GetRelativeScale()[1]));
		//Front
		t.SetScale(Vector3(GetRelativeScale()[0], GetRelativeScale()[1], 1.f));
		t.SetPosition(GetRelativePosition() + Vector3(0.f, 0.f, -GetRelativeScale()[2] / 2.f));
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix() * pt);
		modelManager->Plane()->Render();

		//Back
		t.SetRotation(Vector3(0.f, 180.f, 0.f));
		t.Move(Vector3(0.f, 0.f, GetRelativeScale()[2]));
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix() * pt);
		modelManager->Plane()->Render();

		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(GetRelativeScale()[2], GetRelativeScale()[1]));
		//Left
		t.SetScale(Vector3(GetRelativeScale()[2], GetRelativeScale()[1], 1.f));
		t.SetRotation(Vector3(0.f, 90.f, 0.f));
		t.SetPosition(Vector3(GetRelativePosition() - Vector3(GetRelativeScale()[0] / 2.f, 0.f, 0.f)));
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix() * pt);
		modelManager->Plane()->Render();

		//Right
		t.SetRotation(Vector3(0.f, -90.f, 0.f));
		t.Move(Vector3(GetRelativeScale()[0], 0.f, 0.f));
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix() * pt);
		modelManager->Plane()->Render();


		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(GetRelativeScale()[0], GetRelativeScale()[2]));
		//Bottom
		t.SetScale(Vector3(GetRelativeScale()[0], GetRelativeScale()[2], 1.f));
		t.SetRotation(Vector3(90.f, 0.f, 0.f));
		t.SetPosition(GetRelativePosition() - Vector3(0.f, GetRelativeScale()[1] / 2.f, 0.f));
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix() * pt);
		modelManager->Plane()->Render();

		//Top
		t.SetRotation(Vector3(-90.f, 0.f, 0.f));
		t.Move(Vector3(0.f, GetRelativeScale()[1], 0.f));
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix() * pt);
		modelManager->Plane()->Render();

		//Done
		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(1.f, 1.f));
	}
}

void EntBrush3D::WriteData(BufferWriter<byte> &writer, NumberedSet<String> &strings) const
{
	Entity::WriteData(writer, strings);

	if (Engine::Instance().pMaterialManager && _material)
	{																	//todo: const cast removal
		uint16 id = strings.Add(Engine::Instance().pMaterialManager->FindNameOf(const_cast<Material*>(_material.Ptr())));
		writer.Write_uint16(id);
	}
	else writer.Write_uint16(0);
}

void EntBrush3D::ReadData(BufferReader<byte> &reader, const NumberedSet<String> &strings)
{
	Entity::ReadData(reader, strings);

	const String *materialName = strings.Get(reader.Read_uint16());
	if (materialName)
		SetMaterial(*materialName);

	_point1[0] = GetRelativePosition()[0] - GetRelativeScale()[0] / 2.f;
	_point1[1] = GetRelativePosition()[1] - GetRelativeScale()[1] / 2.f;
	_point1[2] = GetRelativePosition()[2] - GetRelativeScale()[2] / 2.f;
	_point2[0] = GetRelativePosition()[0] + GetRelativeScale()[0] / 2.f;
	_point2[1] = GetRelativePosition()[1] + GetRelativeScale()[1] / 2.f;
	_point2[2] = GetRelativePosition()[2] + GetRelativeScale()[2] / 2.f;
}

const PropertyCollection& EntBrush3D::GetProperties()
{
	static PropertyCollection cvars;

	DO_ONCE_BEGIN;
	_AddBaseProperties(cvars);

	cvars.Add(
		"Material", 
		MemberGetter<EntBrush<3>, String>(&EntBrush<3>::GetMaterialName), 
		MemberSetter<EntBrush<3>, String>(&EntBrush<3>::SetMaterial), 
		0,
		PropertyFlags::MATERIAL);
		
	cvars.Add<Vector3>(
		"Point1", 
		offsetof(EntBrush3D, _point1));

	cvars.Add<Vector3>(
		"Point2", 
		offsetof(EntBrush3D, _point2));
	DO_ONCE_END;

	return cvars;
}
