#include "Connector.h"
#include "DrawUtils.h"
#include "Engine.h"

using namespace DrawUtils;

void Connector::Render() const
{
	if (Engine::modelManager)
	{
		Vector3 p1 = Vector3(point1[0], point1[1], point1[2]);
		Vector3 p2 = Vector3(point2[0], point2[1], point2[2]);
		
		DrawLine(*Engine::modelManager, p1, Vector3(p2[0], p1[1], p1[2]));
		DrawLine(*Engine::modelManager, p1, Vector3(p1[0], p2[1], p1[2]));
		DrawLine(*Engine::modelManager, p1, Vector3(p1[0], p1[1], p2[2]));

		DrawLine(*Engine::modelManager, p2, Vector3(p1[0], p2[1], p2[2]));
		DrawLine(*Engine::modelManager, p2, Vector3(p2[0], p1[1], p2[2]));
		DrawLine(*Engine::modelManager, p2, Vector3(p2[0], p2[1], p1[2]));

		DrawLine(*Engine::modelManager, Vector3(p1[0], p2[1], p1[2]), Vector3(p2[0], p2[1], p1[2]));
		DrawLine(*Engine::modelManager, Vector3(p1[0], p2[1], p1[2]), Vector3(p1[0], p2[1], p2[2]));
		DrawLine(*Engine::modelManager, Vector3(p2[0], p1[1], p2[2]), Vector3(p2[0], p1[1], p1[2]));
		DrawLine(*Engine::modelManager, Vector3(p2[0], p1[1], p2[2]), Vector3(p1[0], p1[1], p2[2]));

		DrawLine(*Engine::modelManager, Vector3(p2[0], p1[1], p1[2]), Vector3(p2[0], p2[1], p1[2]));
		DrawLine(*Engine::modelManager, Vector3(p1[0], p1[1], p2[2]), Vector3(p1[0], p2[1], p2[2]));
	}

}

void Connector::SaveToFile(BufferIterator<byte> &buffer) const
{
	buffer.Write_byte((byte)direction);
	buffer.Write_uint16(point1[0]);
	buffer.Write_uint16(point1[1]);
	buffer.Write_uint16(point1[2]);
	buffer.Write_uint16(point2[0]);
	buffer.Write_uint16(point2[1]);
	buffer.Write_uint16(point2[2]);
}

void Connector::LoadFromFile(BufferIterator<byte> &buffer)
{
	direction = (ConnectorDirection)buffer.Read_byte();
	point1[0] = buffer.Read_uint16();
	point1[1] = buffer.Read_uint16();
	point1[2] = buffer.Read_uint16();
	point2[0] = buffer.Read_uint16();
	point2[1] = buffer.Read_uint16();
	point2[2] = buffer.Read_uint16();
}
