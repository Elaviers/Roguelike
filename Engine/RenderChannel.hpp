#pragma once

enum class RenderChannels : byte
{
	SURFACE = 0x1,
	UNLIT = 0x2,
	SPRITE = 0x4,
	EDITOR = 0x8,

	NONE = 0,
	ALL = 0xFF
};

inline RenderChannels operator|(const RenderChannels& a, const RenderChannels& b)
{
	return RenderChannels(int(a) | int(b));
}

inline RenderChannels operator|=(RenderChannels& a, const RenderChannels& b)
{
	return a = a | b;
}

inline RenderChannels operator~(const RenderChannels& rc)
{
	return RenderChannels(~int(rc));
}

inline bool operator&(const RenderChannels& a, const RenderChannels& b)
{
	return int(a) & int(b);
}
