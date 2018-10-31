#include "Brush2D.h"
#include "GLProgram.h"
#include "Utilities.h"

void Brush2D::_UpdateTransform()
{
	float x = (_point1[0] + _point2[0]) / 2.f;
	float z = (_point1[1] + _point2[1]) / 2.f;
	float w = Maths::Abs(_point1[0] - _point2[0]);
	float h = Maths::Abs(_point1[1] - _point2[1]);

	transform.SetPosition(Vector3(x, level, z));
	transform.SetScale(Vector3(w, h, 0));

	w /= 2.f;
	h /= 2.f;

	_collider.min = Vector3(x - w, level, z - h);
	_collider.max = Vector3(x + w, level, z + h);
}

void Brush2D::Render() const
{
	if (Engine::modelManager && _material)
	{
		_material->Apply();
		this->ApplyTransformToShader();
		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(transform.Scale()[0], transform.Scale()[1]));
		Engine::modelManager->Plane().Render();
		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(1.f, 1.f));
	}
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

	buffer.Write_vector3(transform.Position());

	buffer.Write_float(transform.Scale()[0]);
	buffer.Write_float(transform.Scale()[1]);
}

void Brush2D::LoadFromFile(BufferIterator<byte> &buffer, const Map<uint16, String> &strings)
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

void Brush2D::GetProperties(ObjectProperties &properties)
{
	_AddBaseProperties(properties);

	properties.Add<Brush2D, String>("Material", this, &Brush2D::GetMaterialName, &Brush2D::SetMaterial, PropertyFlags::MATERIAL);
	properties.Add<Vector2>("Point 1", _point1);
	properties.Add<Vector2>("Point 2", _point2);
	properties.Add<float>("Level", level);
}
