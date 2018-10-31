#pragma once
#include "Box.h"
#include "BufferIterator.h"

enum class ConnectorDirection
{
	NORTH = 0,
	EAST = 1,
	SOUTH = 2,
	WEST = 3
};

class Connector : public Box
{
public:
	Connector() : direction(ConnectorDirection::NORTH) {}
	virtual ~Connector() {}

	ConnectorDirection direction;

	void SaveToFile(BufferIterator<byte>&) const;
	void LoadFromFile(BufferIterator<byte>&);
};
