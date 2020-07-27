#pragma once
#include <ELCore/Types.hpp>

enum class ERenderChannels : byte
{
	SURFACE		= 0x01,
	UNLIT		= 0x02,
	SPRITE		= 0x04,
	SKY			= 0x08,
	EDITOR		= 0x10,

	NONE		= 0
};

#include <ELCore/MacroUtilities.hpp>
DEFINE_BITMASK_FUNCS(ERenderChannels, byte)
