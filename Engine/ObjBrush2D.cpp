#include "ObjBrush2D.hpp"
#include "GLProgram.hpp"
#include "MaterialManager.hpp"
#include "ModelManager.hpp"
#include "Utilities.hpp"

void ObjBrush2D::_UpdateTransform()
{
	float x = (_point1[0] + _point2[0]) / 2.f;
	float z = (_point1[1] + _point2[1]) / 2.f;
	float w = Maths::Abs(_point1[0] - _point2[0]);
	float h = Maths::Abs(_point1[1] - _point2[1]);

	SetRelativePosition(Vector3(x, level, z));
	SetRelativeScale(Vector3(w, h, 0));
}

void ObjBrush2D::Render() const
{
	if (Engine::Instance().pModelManager && _material && GLProgram::Current().GetChannels() & _material->GetShaderChannels())
	{
		_material->Apply();
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, GetTransformationMatrix());
		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(GetRelativeScale()[0], GetRelativeScale()[1]));
		Engine::Instance().pModelManager->Plane().Render();
		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(1.f, 1.f));
	}
}


void ObjBrush2D::WriteToFile(BufferIterator<byte> &buffer, NumberedSet<String> &strings) const
{
	if (Engine::Instance().pMaterialManager && _material)
	{
		uint16 id = strings.Add(Engine::Instance().pMaterialManager->FindNameOf(_material));
		buffer.Write_uint16(id);
	}
	else buffer.Write_uint16(0);

	buffer.Write_vector3(GetRelativePosition());

	buffer.Write_float(GetRelativeScale()[0]);
	buffer.Write_float(GetRelativeScale()[1]);
}

void ObjBrush2D::ReadFromFile(BufferIterator<byte> &buffer, const NumberedSet<String> &strings)
{
	const String *materialName = strings.Find(buffer.Read_uint16());
	if (materialName)
		SetMaterial(*materialName);

	SetRelativePosition(buffer.Read_vector3());

	float scaleX = buffer.Read_float();
	float scaleY = buffer.Read_float();

	SetRelativeScale(Vector3(scaleX, scaleY, 0.f));

	level = GetRelativePosition()[1];
	_point1[0] = GetRelativePosition()[0] - GetRelativeScale()[0] / 2.f;
	_point1[1] = GetRelativePosition()[2] - GetRelativeScale()[1] / 2.f;
	_point2[0] = GetRelativePosition()[0] + GetRelativeScale()[0] / 2.f;
	_point2[1] = GetRelativePosition()[2] + GetRelativeScale()[1] / 2.f;
}

void ObjBrush2D::GetCvars(CvarMap &cvars)
{
	_AddBaseCvars(cvars);
	
	cvars.Add("Material", Getter<String>((ObjBrush<2>*)this, &ObjBrush<2>::GetMaterialName), Setter<String>((ObjBrush<2>*)this, &ObjBrush<2>::SetMaterial), PropertyFlags::MATERIAL);
	cvars.Add("Point 1", _point1);
	cvars.Add("Point 2", _point2);
	cvars.Add("Level", level);
}
