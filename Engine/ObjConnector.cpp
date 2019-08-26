#include "ObjConnector.hpp"
#include "DrawUtils.hpp"
#include "Engine.hpp"
#include "GLProgram.hpp"
#include "ModelManager.hpp"
#include "TextureManager.hpp"

void ObjConnector::Render(EnumRenderChannel channels) const {
	if (Engine::Instance().pModelManager && channels & RenderChannel::UNLIT)
	{
		Engine::Instance().pTextureManager->White()->Bind(0);

		Vector3 c = (_min + _max) / 2.f;
		Vector3 size = _max - _min;

		glLineWidth(3);
		GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, _colour);
		DrawUtils::DrawBox(*Engine::Instance().pModelManager, _min, _max);

		Transform t;

		switch (direction)
		{
		case Direction2D::NORTH:
			t.SetPosition(Vector3(c[0], c[1], _max[2]));
			t.SetScale(Vector3(size[0], size[1], 1.f));
			break;

		case Direction2D::SOUTH:
			t.SetPosition(Vector3(c[0], c[1], _min[2]));
			t.SetScale(Vector3(size[0], size[1], 1.f));
			break;

		case Direction2D::EAST:
			t.SetPosition(Vector3(_max[0], c[1], c[2]));
			t.SetScale(Vector3(size[2], size[1], 1.f));
			t.SetRotation(Vector3(0.f, 90.f, 0.f));
			break;

		case Direction2D::WEST:
			t.SetPosition(Vector3(_min[0], c[1], c[2]));
			t.SetScale(Vector3(size[2], size[1], 1.f));
			t.SetRotation(Vector3(0.f, 90.f, 0.f));
			break;
		}

		GLProgram::Current().SetMat4(DefaultUniformVars::mat4Model, t.MakeTransformationMatrix());

		glDisable(GL_CULL_FACE);
		Engine::Instance().pModelManager->Plane().Render();
		glEnable(GL_CULL_FACE);
	}
}

void ObjConnector::WriteData(BufferWriter<byte> &writer, NumberedSet<String> &strings) const
{
	GameObject::WriteData(writer, strings);

	writer.Write_byte((byte)direction);
	writer.Write_vector3(_min);
	writer.Write_vector3(_max);
}

void ObjConnector::ReadData(BufferReader<byte> &reader, const NumberedSet<String> &strings)
{
	GameObject::ReadData(reader, strings);

	direction = (Direction2D)reader.Read_byte();
	_min = reader.Read_vector3();
	_max = reader.Read_vector3();
}
