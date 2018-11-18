#pragma once
#include "BufferIterator.h"
#include "Connector.h"
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

class Level
{
	Collection _collection;
	Buffer<Connector> _connectors;

public:
	void WriteToFile(const char *filename) const;
	void ReadFromFile(const char *filename);

	inline void Clear()
	{
		_collection.Clear();
		_connectors.SetSize(0);
	}

	inline Collection& ObjectCollection() { return _collection; }
	inline const Collection& ObjectCollection() const { return _collection; }

	inline Buffer<Connector>& Connectors() { return _connectors; }
};
