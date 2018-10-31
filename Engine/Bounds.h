#pragma once
#include "Vector.h"

#undef min
#undef max

struct Bounds
{
	Vector3 min;
	Vector3 max;

	Bounds(const Vector3 &min = Vector3(), const Vector3 &max = Vector3()) : min(min), max(max) {}
};
