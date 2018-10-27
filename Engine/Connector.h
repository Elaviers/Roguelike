#pragma once
#include "BufferIterator.h"
#include "Vector.h"

enum class ConnectorDirection
{
	NORTH = 0,
	EAST = 1,
	SOUTH = 2,
	WEST = 3
};

class Connector
{
public:
	Connector() : direction(ConnectorDirection::NORTH) {}
	~Connector() {}

	Vector<int16, 3> point1;
	Vector<int16, 3> point2;

	ConnectorDirection direction;

	void Render() const;

	void SaveToFile(BufferIterator<byte>&) const;
	void LoadFromFile(BufferIterator<byte>&);
};
