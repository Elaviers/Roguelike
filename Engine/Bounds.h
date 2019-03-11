#pragma once
#include "Vector.h"

#undef min
#undef max

/*
	Bounds

	AABB/Sphere
*/

struct Bounds
{
	//AABB
	Vector3 min;
	Vector3 max;

	//Sphere
	Vector3 centre;
	float radius;

	Bounds(const Vector3 &min = Vector3(), const Vector3 &max = Vector3()) : radius(0.f), min(min), max(max), centre((max - min) / 2.f) { RecalculateSphereBounds(); }

	void RecalculateSphereBounds();
};
