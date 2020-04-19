#include "EntBrush3D.hpp"
#include "GLProgram.hpp"
#include "MacroUtilities.hpp"
#include "Utilities.hpp"

void EntBrush3D::_OnTransformChanged()
{
	if (!_updatingTransform)
	{
		float hw = GetRelativeScale().x / 2.f;
		float hh = GetRelativeScale().y / 2.f;
		float hd = GetRelativeScale().z / 2.f;

		_point1 = Vector3(GetRelativePosition().x - hw, GetRelativePosition().y - hh, GetRelativePosition().z - hd);
		_point2 = Vector3(GetRelativePosition().x + hw, GetRelativePosition().y + hh, GetRelativePosition().z + hd);
	}
}

void EntBrush3D::_OnPointChanged()
{
	float x = (_point1.x + _point2.x) / 2.f;
	float y = (_point1.y + _point2.y) / 2.f;
	float z = (_point1.z + _point2.z) / 2.f;
	float w = Maths::Abs(_point1.x - _point2.x);
	float h = Maths::Abs(_point1.y - _point2.y);
	float d = Maths::Abs(_point1.z - _point2.z);

	_updatingTransform = true;
	SetRelativePosition(Vector3(x, y, z));
	SetRelativeScale(Vector3(w, h, d));
	_updatingTransform = false;
}

#include "DrawUtils.hpp"

void EntBrush3D::Render(ERenderChannels channels) const
{
	ModelManager* modelManager = Engine::Instance().pModelManager;

	if (modelManager && _material && channels & _material->GetRenderChannelss())
	{
		_material->Apply();

		Mat4 pt = _parent ? _parent->GetTransformationMatrix() : Mat4();

		Transform t;

		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(GetRelativeScale().x, GetRelativeScale().y));
		//Front
		t.SetScale(Vector3(GetRelativeScale().x, GetRelativeScale().y, 1.f));
		t.SetPosition(GetRelativePosition() + Vector3(0.f, 0.f, -GetRelativeScale().z / 2.f));
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix() * pt);
		modelManager->Plane()->Render();

		//Back
		t.SetRotation(Vector3(0.f, 180.f, 0.f));
		t.Move(Vector3(0.f, 0.f, GetRelativeScale().z));
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix() * pt);
		modelManager->Plane()->Render();

		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(GetRelativeScale().z, GetRelativeScale().y));
		//Left
		t.SetScale(Vector3(GetRelativeScale().z, GetRelativeScale().y, 1.f));
		t.SetRotation(Vector3(0.f, 90.f, 0.f));
		t.SetPosition(Vector3(GetRelativePosition() - Vector3(GetRelativeScale().x / 2.f, 0.f, 0.f)));
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix() * pt);
		modelManager->Plane()->Render();

		//Right
		t.SetRotation(Vector3(0.f, -90.f, 0.f));
		t.Move(Vector3(GetRelativeScale().x, 0.f, 0.f));
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix() * pt);
		modelManager->Plane()->Render();


		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(GetRelativeScale().x, GetRelativeScale().z));
		//Bottom
		t.SetScale(Vector3(GetRelativeScale().x, GetRelativeScale().z, 1.f));
		t.SetRotation(Vector3(90.f, 0.f, 0.f));
		t.SetPosition(GetRelativePosition() - Vector3(0.f, GetRelativeScale().y / 2.f, 0.f));
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix() * pt);
		modelManager->Plane()->Render();

		//Top
		t.SetRotation(Vector3(-90.f, 0.f, 0.f));
		t.Move(Vector3(0.f, GetRelativeScale().y, 0.f));
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

	_point1.x = GetRelativePosition().x - GetRelativeScale().x / 2.f;
	_point1.y = GetRelativePosition().y - GetRelativeScale().y / 2.f;
	_point1.z = GetRelativePosition().z - GetRelativeScale().z / 2.f;
	_point2.x = GetRelativePosition().x + GetRelativeScale().x / 2.f;
	_point2.y = GetRelativePosition().y + GetRelativeScale().y / 2.f;
	_point2.z = GetRelativePosition().z + GetRelativeScale().z / 2.f;
}

const PropertyCollection& EntBrush3D::GetProperties()
{
	static PropertyCollection cvars;

	DO_ONCE_BEGIN;
	_AddBaseProperties(cvars);

	cvars.Add(
		"Material", 
		MemberGetter<EntBrush3D, String>(&EntBrush3D::GetMaterialName), 
		MemberSetter<EntBrush3D, String>(&EntBrush3D::SetMaterial), 
		0,
		PropertyFlags::MATERIAL);
		
	cvars.Add(
		"Point1", 
		MemberGetter<EntBrush3D, const Vector3&>(&EntBrush3D::GetPoint1),
		MemberSetter<EntBrush3D, Vector3>(&EntBrush3D::SetPoint1));

	cvars.Add(
		"Point2",
		MemberGetter<EntBrush3D, const Vector3&>(&EntBrush3D::GetPoint2),
		MemberSetter<EntBrush3D, Vector3>(&EntBrush3D::SetPoint2));

	DO_ONCE_END;

	return cvars;
}
