#pragma once

namespace RenderChannel
{
	enum RenderChannel : byte
	{
		SURFACE = 0x1,
		UNLIT = 0x2,
		SPRITE = 0x4,
		EDITOR = 0x8,

		NONE = 0,
		ALL = 0xFF
	};
}

typedef RenderChannel::RenderChannel EnumRenderChannel;
