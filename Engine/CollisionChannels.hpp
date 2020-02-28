#pragma once
#include "Types.hpp"

enum class ECollisionChannels : byte
{
	ALL =		0xFF,
	STATIC =	0x01,		//Static Geometry
	DYNAMIC =	0x02,		//Dynamic (movable) Geometry
	PROP =		0x04,		//Physics / small object
	PLAYER =	0x08,
	EDITOR =	0x80,

	SURFACE = STATIC | DYNAMIC | PROP | PLAYER
};

#include "MacroUtilities.hpp"
DEFINE_BITMASK_FUNCS(ECollisionChannels, byte)
