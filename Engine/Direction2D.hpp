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
	//Gets the angle in degrees represented by a direction (0° == east)
	int GetAngleOf(Direction2D dir);
}
