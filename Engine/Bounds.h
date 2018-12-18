#pragma once
#include "Vector.h"

#undef min
#undef max

struct Bounds
{
	Vector3 min;
	Vector3 max;
	Vector3 centre;
	float radius;

	Bounds(const Vector3 &min = Vector3(), const Vector3 &max = Vector3()) : radius(radius), min(min), max(max), centre((max - min) / 2.f) { RecalculateSphereBounds(); }

	void RecalculateSphereBounds();
};
