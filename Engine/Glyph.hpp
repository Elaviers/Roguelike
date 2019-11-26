#pragma once
#include "Vector.hpp"

struct Glyph
{
	Vector2 uv;
	Vector2 uvSize;

	int width = 0;
	int advance = 0;
};
