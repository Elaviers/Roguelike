#include "ObjBrush3D.h"
#include "GLProgram.h"
#include "Utilities.h"

void ObjBrush3D::_UpdateTransform()
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

#include "DrawUtils.h"

void ObjBrush3D::Render() const
{
	ModelManager* modelManager = Engine::Instance().pModelManager;

	if (modelManager && _material && GLProgram::Current().GetChannels() & _material->GetShaderChannels())
	{
		_material->Apply();

		Mat4 pt = _parent ? _parent->GetTransformationMatrix() : Mat4();

		Transform t;

		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(GetRelativeScale()[0], GetRelativeScale()[1]));
		//Front
		t.SetScale(Vector3(GetRelativeScale()[0], GetRelativeScale()[1], 0.f));
		t.SetPosition(GetRelativePosition() + Vector3(0.f, 0.f, -GetRelativeScale()[2] / 2.f));
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix() * pt);
		modelManager->Plane().Render();

		//Back
		t.SetRotation(Vector3(0.f, 180.f, 0.f));
		t.Move(Vector3(0.f, 0.f, GetRelativeScale()[2]));
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix() * pt);
		modelManager->Plane().Render();

		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(GetRelativeScale()[2], GetRelativeScale()[1]));
		//Left
		t.SetScale(Vector3(GetRelativeScale()[2], GetRelativeScale()[1], 0.f));
		t.SetRotation(Vector3(0.f, 90.f, 0.f));
		t.SetPosition(Vector3(GetRelativePosition() - Vector3(GetRelativeScale()[0] / 2.f, 0.f, 0.f)));
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix() * pt);
		modelManager->Plane().Render();

		//Right
		t.SetRotation(Vector3(0.f, -90.f, 0.f));
		t.Move(Vector3(GetRelativeScale()[0], 0.f, 0.f));
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix() * pt);
		modelManager->Plane().Render();


		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(GetRelativeScale()[0], GetRelativeScale()[2]));
		//Bottom
		t.SetScale(Vector3(GetRelativeScale()[0], GetRelativeScale()[2], 0.f));
		t.SetRotation(Vector3(90.f, 0.f, 0.f));
		t.SetPosition(GetRelativePosition() - Vector3(0.f, GetRelativeScale()[1] / 2.f, 0.f));
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix() * pt);
		modelManager->Plane().Render();

		//Top
		t.SetRotation(Vector3(-90.f, 0.f, 0.f));
		t.Move(Vector3(0.f, GetRelativeScale()[1], 0.f));
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix() * pt);
		modelManager->Plane().Render();

		//Done
		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(1.f, 1.f));
	}
}

void ObjBrush3D::WriteToFile(BufferIterator<byte> &buffer, NumberedSet<String> &strings) const
{
	if (Engine::Instance().pMaterialManager && _material)
	{																	//todo: const cast removal
		uint16 id = strings.Add(Engine::Instance().pMaterialManager->FindNameOf(const_cast<Material*>(_material)));
		buffer.Write_uint16(id);
	}
	else buffer.Write_uint16(0);

	buffer.Write_vector3(GetRelativePosition());
	buffer.Write_vector3(GetRelativeScale());
}

void ObjBrush3D::ReadFromFile(BufferIterator<byte> &buffer, const NumberedSet<String> &strings)
{
	const String *materialName = strings.Find(buffer.Read_uint16());
	if (materialName)
		SetMaterial(*materialName);

	SetRelativePosition(buffer.Read_vector3());
	SetRelativeScale(buffer.Read_vector3());

	_point1[0] = GetRelativePosition()[0] - GetRelativeScale()[0] / 2.f;
	_point1[1] = GetRelativePosition()[1] - GetRelativeScale()[1] / 2.f;
	_point1[2] = GetRelativePosition()[2] - GetRelativeScale()[2] / 2.f;
	_point2[0] = GetRelativePosition()[0] + GetRelativeScale()[0] / 2.f;
	_point2[1] = GetRelativePosition()[1] + GetRelativeScale()[1] / 2.f;
	_point2[2] = GetRelativePosition()[2] + GetRelativeScale()[2] / 2.f;
}

void ObjBrush3D::GetCvars(CvarMap &cvar)
{
	_AddBaseCvars(cvar);

	cvar.Add("Material", Getter<String>((ObjBrush<3>*)this, &ObjBrush<3>::GetMaterialName), Setter<String>((ObjBrush<3>*)this, &ObjBrush<3>::SetMaterial), PropertyFlags::MATERIAL);
	cvar.Add("Point 1", _point1);
	cvar.Add("Point 2", _point2);
}
