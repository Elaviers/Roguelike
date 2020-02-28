#pragma once
#include "Types.hpp"

enum class ERenderChannels : byte
{
	PRE_RENDER = 0x80,

	SURFACE = 0x1,
	UNLIT = 0x2,
	SPRITE = 0x4,
	EDITOR = 0x8,

	NONE = 0,
	ALL = 0xFF
};

#include "MacroUtilities.hpp"
DEFINE_BITMASK_FUNCS(ERenderChannels, byte)
