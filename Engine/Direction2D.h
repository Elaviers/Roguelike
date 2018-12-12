#pragma once

enum class Direction2D
{
	NORTH = 0,
	EAST = 1,
	SOUTH = 2,
	WEST = 3
};

namespace Direction2DFuncs
{
	int GetAngleOf(Direction2D dir);
}
