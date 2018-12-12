#include "Direction2D.h"

namespace Direction2DFuncs
{
	int GetAngleOf(Direction2D dir)
	{
		switch (dir)
		{
		case Direction2D::NORTH:
			return 90;
		case Direction2D::SOUTH:
			return -90;
		case Direction2D::WEST:
			return 180;
		default:
			return 0;
		}
	}
}
