#include "Brush2D.h"
#include "Engine.h"
#include "GLProgram.h"
#include "Maths.h"
#include "ObjectProperties.h"

void Brush2D::_UpdateTransform()
{
	float x = (_point1[0] + _point2[0]) / 2.f;
	float z = (_point1[1] + _point2[1]) / 2.f;
	float w = Maths::Abs(_point1[0] - _point2[0]);
	float h = Maths::Abs(_point1[1] - _point2[1]);

	transform.SetPosition(Vector3(x, level, z));
	transform.SetScale(Vector3(w, h, 0));
}

void Brush2D::GetProperties(ObjectProperties &properties)
{
	_AddBaseProperties(properties);

	properties.Add<Brush2D, String>("Material", this, &Brush2D::GetMaterialName, &Brush2D::SetMaterial, PropertyFlags::MATERIAL);
	properties.Add<Vector2>("Point 1", _point1);
	properties.Add<Vector2>("Point 2", _point2);
	properties.Add<float>("Level", level);
}

void Brush2D::Render() const
{
	if (Engine::modelManager && _material)
	{
		_material->Apply();
		this->ApplyTransformToShader();
		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(transform.GetScale()[0], transform.GetScale()[1]));
		Engine::modelManager->Plane().Render();
		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(1.f, 1.f));
	}
}

String Brush2D::GetMaterialName() const
{
	if (Engine::materialManager && _material) return Engine::materialManager->FindNameOf(*_material);
	return "Unknown";
}

void Brush2D::SaveToFile(BufferIterator<byte> &buffer, const Map<String, uint16> &strings) const
{
	buffer.Write_byte(Engine::ObjectIDs::BRUSH2D);
	
	if (Engine::materialManager && _material)
	{
		const uint16 *id = strings.Find(Engine::materialManager->FindNameOf(*_material));
		if (id) buffer.Write_uint16(*id);
		else buffer.Write_uint16(0);
	}
	else buffer.Write_uint16(0);

	Vector3 pos = transform.GetPosition();
	buffer.Write_float(pos[0]);
	buffer.Write_float(pos[1]);
	buffer.Write_float(pos[2]);

	Vector3 scale = transform.GetScale();
	buffer.Write_float(scale[0]);
	buffer.Write_float(scale[1]);
}

void Brush2D::LoadFromFile(BufferIterator<byte> &buffer, const Map<uint16, String> &strings)
{
	const String *materialName = strings.Find(buffer.Read_uint16());
	if (materialName)
		SetMaterial(*materialName);

	Vector3 pos;
	pos[0] = buffer.Read_float();
	pos[1] = buffer.Read_float();
	pos[2] = buffer.Read_float();
	transform.SetPosition(pos);

	Vector3 scale;
	scale[0] = buffer.Read_float();
	scale[1] = buffer.Read_float();
	transform.SetScale(scale);

	level = pos[1];
	_point1[0] = pos[0] - scale[0] / 2.f;
	_point1[1] = pos[2] - scale[1] / 2.f;
	_point2[0] = pos[0] + scale[0] / 2.f;
	_point1[1] = pos[2] + scale[1] / 2.f;
}
