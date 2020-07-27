#pragma once
#include "Vector2.hpp"

struct UVRect
{
	Vector2 pos;
	Vector2 size;

	UVRect(float x = 0.f, float y = 0.f, float w = 1.f, float h = 1.f) : pos(x, y), size(w, h) {}
};
