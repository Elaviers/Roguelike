#pragma once
#include "Buffer.h"
#include "Types.h"

namespace LevelMessages
{
	enum Enum
	{
		LAYER = 0,
		X = 1,
		Y = 2,
		PLANE = 3,
		RAMP = 4,
		CONNECTOR = 5,
		MATERIAL = 6,
		MATERIALID = 7
	};
}

namespace LevelDirections
{
	enum Enum
	{
		NORTH = 0,
		EAST = 1,
		SOUTH = 2,
		WEST = 3,

		NORTHEAST = 4,
		SOUTHEAST = 5,
		SOUTHWEST = 6,
		NORTHWEST = 7
	};
}

enum class LevelObjectType
{
	TILE = 0,
	PLANE = 1,
	RAMP = 2,
	CONNECTOR = 3
};

struct LevelObject
{
	LevelObjectType type;
	byte x, y;

	union
	{
		struct { byte material; }						tileData;
		struct { byte w, h, material; }					planeData;
		struct { byte dir, material; }					rampData;
		struct { byte w, h; }							connectorData;
	};
};

namespace LevelIO
{
	Buffer<LevelObject> ReadLevel(const char *filename);
}
