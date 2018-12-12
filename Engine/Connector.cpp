#include "Connector.h"
#include "DrawUtils.h"
#include "Engine.h"
#include "GLProgram.h"

void Connector::Render() const {
	if (Engine::modelManager)
	{
		Vector3 c = (_min + _max) / 2.f;
		Vector3 size = _max - _min;

		DrawUtils::DrawBox(*Engine::modelManager, _min, _max);

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
		Engine::modelManager->Plane().Render();
		glEnable(GL_CULL_FACE);
	}
}

void Connector::SaveToFile(BufferIterator<byte> &buffer) const
{
	buffer.Write_byte((byte)direction);
	buffer.Write_vector3(_min);
	buffer.Write_vector3(_max);
}

void Connector::LoadFromFile(BufferIterator<byte> &buffer)
{
	direction = (Direction2D)buffer.Read_byte();
	_min = buffer.Read_vector3();
	_max = buffer.Read_vector3();
}
