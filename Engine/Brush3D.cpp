#include "Brush3D.h"
#include "GLProgram.h"
#include "Utilities.h"

void Brush3D::_OnTransformChanged()
{
	Vector3 halfSz = transform.Scale() / 2.f;

	_collider.min = transform.Position() - halfSz;
	_collider.max = transform.Position() + halfSz;
}

void Brush3D::_UpdateTransform()
{
	float x = (_point1[0] + _point2[0]) / 2.f;
	float y = (_point1[1] + _point2[1]) / 2.f;
	float z = (_point1[2] + _point2[2]) / 2.f;
	float w = Maths::Abs(_point1[0] - _point2[0]);
	float h = Maths::Abs(_point1[1] - _point2[1]);
	float d = Maths::Abs(_point1[2] - _point2[2]);

	transform.SetPosition(Vector3(x, y, z));
	transform.SetScale(Vector3(w, h, d));
}

#include "DrawUtils.h"

void Brush3D::Render() const
{
	if (Engine::modelManager && _material)
	{
		_material->Apply();

		Transform t;

		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(transform.Scale()[0], transform.Scale()[1]));
		//Front
		t.SetScale(Vector3(transform.Scale()[0], transform.Scale()[1], 0.f));
		t.SetPosition(transform.GetPosition() + Vector3(0.f, 0.f, -transform.Scale()[2] / 2.f));
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix());
		Engine::modelManager->Plane().Render();

		//Back
		t.SetRotation(Vector3(0.f, 180.f, 0.f));
		t.Move(Vector3(0.f, 0.f, transform.Scale()[2]));
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix());
		Engine::modelManager->Plane().Render();

		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(transform.Scale()[2], transform.Scale()[1]));
		//Left
		t.SetScale(Vector3(transform.Scale()[2], transform.Scale()[1], 0.f));
		t.SetRotation(Vector3(0.f, 90.f, 0.f));
		t.SetPosition(Vector3(transform.GetPosition() - Vector3(transform.Scale()[0] / 2.f, 0.f, 0.f)));
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix());
		Engine::modelManager->Plane().Render();

		//Right
		t.SetRotation(Vector3(0.f, -90.f, 0.f));
		t.Move(Vector3(transform.Scale()[0], 0.f, 0.f));
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix());
		Engine::modelManager->Plane().Render();


		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(transform.Scale()[0], transform.Scale()[2]));
		//Bottom
		t.SetScale(Vector3(transform.Scale()[0], transform.Scale()[2], 0.f));
		t.SetRotation(Vector3(90.f, 0.f, 0.f));
		t.SetPosition(transform.GetPosition() - Vector3(0.f, transform.Scale()[1] / 2.f, 0.f));
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix());
		Engine::modelManager->Plane().Render();

		//Top
		t.SetRotation(Vector3(-90.f, 0.f, 0.f));
		t.Move(Vector3(0.f, transform.Scale()[1], 0.f));
		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix());
		Engine::modelManager->Plane().Render();

		//Done
		GLProgram::Current().SetVec2(DefaultUniformVars::vec2UVScale, Vector2(1.f, 1.f));
	}
}

void Brush3D::SaveToFile(BufferIterator<byte> &buffer, const Map<String, uint16> &strings) const
{
	buffer.Write_byte(Engine::ObjectIDs::BRUSH3D);

	if (Engine::materialManager && _material)
	{																	//todo: const cast removal
		const uint16 *id = strings.Find(Engine::materialManager->FindNameOf(const_cast<Material*>(_material)));
		if (id) buffer.Write_uint16(*id);
		else buffer.Write_uint16(0);
	}
	else buffer.Write_uint16(0);

	buffer.Write_vector3(transform.Position());
	buffer.Write_vector3(transform.Scale());
}

void Brush3D::LoadFromFile(BufferIterator<byte> &buffer, const Map<uint16, String> &strings)
{
	const String *materialName = strings.Find(buffer.Read_uint16());
	if (materialName)
		SetMaterial(*materialName);

	transform.SetPosition(buffer.Read_vector3());
	transform.SetScale(buffer.Read_vector3());

	_point1[0] = transform.Position()[0] - transform.Scale()[0] / 2.f;
	_point1[1] = transform.Position()[1] - transform.Scale()[1] / 2.f;
	_point1[2] = transform.Position()[2] - transform.Scale()[2] / 2.f;
	_point2[0] = transform.Position()[0] + transform.Scale()[0] / 2.f;
	_point2[1] = transform.Position()[1] + transform.Scale()[1] / 2.f;
	_point2[2] = transform.Position()[2] + transform.Scale()[2] / 2.f;
}

void Brush3D::GetProperties(ObjectProperties &properties)
{
	_AddBaseProperties(properties);

	properties.Add<Brush3D, String>("Material", this, &Brush3D::GetMaterialName, &Brush3D::SetMaterial, PropertyFlags::MATERIAL);
	properties.Add<Vector3>("Point 1", _point1);
	properties.Add<Vector3>("Point 2", _point2);
}
