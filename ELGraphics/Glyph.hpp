#pragma once
#include <ELMaths/Vector2.hpp>

struct Glyph
{
	Vector2 uv;
	Vector2 uvSize;

	int width = 0;
	int advance = 0;
};
