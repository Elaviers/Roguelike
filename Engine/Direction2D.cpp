#include "EDirection2D.hpp"

namespace Direction2DFuncs
{
	int GetAngleOf(EDirection2D dir)
	{
		switch (dir)
		{
		case EDirection2D::NORTH:
			return 90;
		case EDirection2D::SOUTH:
			return -90;
		case EDirection2D::WEST:
			return 180;
		default:
			return 0;
		}
	}
}
