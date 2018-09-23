#pragma once
#include "BufferIterator.h"
#include "Types.h"
#include "Vector.h"
#include "Collection.h"

namespace LevelVersions
{
	enum //This should be a byte... hopefully I won't need more than 256 level versions.
	{
		VERSION_1 = 0

	};
}

enum class ConnectorDirection
{
	NORTH = 0,
	EAST = 1,
	SOUTH = 2,
	WEST = 3
};

struct Connector
{
	Vector<int16, 2> point1;
	Vector<int16, 2> point2;

	ConnectorDirection direction;

	void SaveToFile(BufferIterator<byte>&) const;
	void LoadFromFile(BufferIterator<byte>&);
};

class Level
{
	Collection _collection;
	Buffer<Connector> _connectors;

public:
	void WriteToFile(const char *filename) const;
	void ReadFromFile(const char *filename);

	inline Collection& ObjectCollection() { return _collection; }
	inline const Collection& ObjectCollection() const { return _collection; }
};
