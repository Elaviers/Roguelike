#include "ObjBrush2D.h"
#include "GLProgram.h"
#include "Utilities.h"

void ObjBrush2D::_UpdateTransform()
{
	float x = (_point1[0] + _point2[0]) / 2.f;
	float z = (_point1[1] + _point2[1]) / 2.f;
	float w = Maths::Abs(_point1[0] - _point2[0]);
	float h = Maths::Abs(_point1[1] - _point2[1]);

	transform.SetPosition(Vector3(x, level, z));
	transform.SetScale(Vector3(w, h, 0));
}

void ObjBrush2D::Render() const
{
	if (Engine::modelManager && _material && GLProgram::Current().GetChannels() & _material->GetShaderChannels())
	{
		_material->Apply();
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, GetTransformationMatrix());
		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(transform.Scale()[0], transform.Scale()[1]));
		Engine::modelManager->Plane().Render();
		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(1.f, 1.f));
	}
}


void ObjBrush2D::WriteToFile(BufferIterator<byte> &buffer, NumberedSet<String> &strings) const
{
	if (Engine::materialManager && _material)
	{																		//TODO: const cast removal
		uint16 id = strings.Add(Engine::materialManager->FindNameOf(const_cast<Material*>(_material)));
		buffer.Write_uint16(id);
	}
	else buffer.Write_uint16(0);

	buffer.Write_vector3(transform.Position());

	buffer.Write_float(transform.Scale()[0]);
	buffer.Write_float(transform.Scale()[1]);
}

void ObjBrush2D::ReadFromFile(BufferIterator<byte> &buffer, const NumberedSet<String> &strings)
{
	const String *materialName = strings.Find(buffer.Read_uint16());
	if (materialName)
		SetMaterial(*materialName);

	transform.SetPosition(buffer.Read_vector3());

	float scaleX = buffer.Read_float();
	float scaleY = buffer.Read_float();

	transform.SetScale(Vector3(scaleX, scaleY, 0.f));

	level = transform.Position()[1];
	_point1[0] = transform.Position()[0] - transform.Scale()[0] / 2.f;
	_point1[1] = transform.Position()[2] - transform.Scale()[1] / 2.f;
	_point2[0] = transform.Position()[0] + transform.Scale()[0] / 2.f;
	_point2[1] = transform.Position()[2] + transform.Scale()[1] / 2.f;
}

void ObjBrush2D::GetCvars(CvarMap &cvars)
{
	_AddBaseCvars(cvars);
	
	cvars.Add("Material", Getter<String>((ObjBrush<2>*)this, &ObjBrush<2>::GetMaterialName), Setter<String>((ObjBrush<2>*)this, &ObjBrush<2>::SetMaterial), PropertyFlags::MATERIAL);
	cvars.Add("Point 1", _point1);
	cvars.Add("Point 2", _point2);
	cvars.Add("Level", level);
}
