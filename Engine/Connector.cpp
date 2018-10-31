#include "Connector.h"

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
