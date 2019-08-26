#pragma once
#include "Vector.hpp"

struct Glyph
{
	Vector2 uvOffset;
	Vector2 uvSize;

	int width = 0;
	int advance = 0;
};
