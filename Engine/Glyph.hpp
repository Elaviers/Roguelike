#pragma once
#include "Vector.hpp"

struct Glyph
{
	Vector2 uvOffset;
	Vector2 uvSize;

	int width;
	int advance;
};
