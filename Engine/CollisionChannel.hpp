#pragma once
#include "Types.hpp"

enum class CollisionChannels : byte
{
	ALL = 0xFF,
	STATIC = 0x01,		//Static Geometry
	DYNAMIC = 0x02,		//Dynamic (movable) Geometry
	PROP = 0x04,		//Physics / small object
	PLAYER = 0x08,
	EDITOR = 0x0F,

	SURFACE = STATIC | DYNAMIC | PROP | PLAYER
};

inline CollisionChannels operator|(const CollisionChannels& a, const CollisionChannels& b)
{
	return CollisionChannels(int(a) | int(b));
}

inline CollisionChannels& operator|=(CollisionChannels& a, const CollisionChannels& b)
{
	return a = a | b;
}

inline CollisionChannels operator~(const CollisionChannels& cc)
{
	return CollisionChannels(~int(cc));
}

inline bool operator&(const CollisionChannels& a, const CollisionChannels& b)
{
	return int(a) & int(b);
}
