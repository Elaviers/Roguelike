#pragma once
#include "Types.hpp"

enum CollisionChannel : byte
{
	COLL_ALL_CHANNELS = 0xFF,
	COLL_STATIC = 0x01,			//Static Geometry
	COLL_DYNAMIC = 0x02,		//Dynamic (movable) Geometry
	COLL_PROP = 0x04,			//Physics / small object
	COLL_PLAYER = 0x08,
	COLL_EDITOR = 0x0F,

	COLL_SURFACE = COLL_STATIC | COLL_DYNAMIC | COLL_PROP | COLL_PLAYER
};
