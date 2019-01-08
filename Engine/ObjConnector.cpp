#include "ObjConnector.h"
#include "DrawUtils.h"
#include "Engine.h"
#include "GLProgram.h"
#include "ShaderChannel.h"

void ObjConnector::Render() const {
	if (Engine::modelManager && GLProgram::Current().GetChannels() & ShaderChannel::UNLIT)
	{
		Engine::textureManager->White().Bind(0);

		Vector3 c = (_min + _max) / 2.f;
		Vector3 size = _max - _min;

		glLineWidth(3);
		GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, Vector4(0.f, 1.f, 0.f, 1.f));
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

void ObjConnector::WriteToFile(BufferIterator<byte> &buffer, NumberedSet<String> &strings) const
{
	buffer.Write_byte((byte)direction);
	buffer.Write_vector3(_min);
	buffer.Write_vector3(_max);
}

void ObjConnector::ReadFromFile(BufferIterator<byte> &buffer, const NumberedSet<String> &strings)
{
	direction = (Direction2D)buffer.Read_byte();
	_min = buffer.Read_vector3();
	_max = buffer.Read_vector3();
}
