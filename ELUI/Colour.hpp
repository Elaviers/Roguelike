#pragma once
#include <ELGraphics/Colour.hpp>

class RenderEntry;

class UIColour
{
	bool _isBlend;

	union
	{
		Colour _colour;
		Colour _blendFrom;
	};

	Colour _blendTo;

public:
	UIColour(const Colour& colour = Colour::White) : _colour(colour), _isBlend(false) {}
	UIColour(const Colour& blendFrom, const Colour& blendTo) : _blendFrom(blendFrom), _blendTo(blendTo), _isBlend(true) {}
	~UIColour() {}

	UIColour(const UIColour& other) : _isBlend(other._isBlend), _blendFrom(other._blendFrom), _blendTo(other._blendTo) {}

	UIColour& operator=(const UIColour& other)
	{
		_isBlend = other._isBlend;
		_blendFrom = other._blendFrom;
		_blendTo = other._blendTo;
		
		return *this;
	}

	void Apply(RenderEntry&) const;
};
