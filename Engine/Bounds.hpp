#pragma once
#include "Vector3.hpp"

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

	Bounds(const Vector3 &min, const Vector3 &max) : radius(0.f), min(min), max(max), centre((max - min) / 2.f) { RecalculateSphereBounds(); }
	Bounds(const Vector3& extent = Vector3()) : radius(0.f), min(-1.f * extent), max(extent) { RecalculateSphereBounds(); }
	Bounds(const float radius) : radius(radius), min(-radius, -radius, -radius), max(radius, radius, radius) {}

	void RecalculateSphereBounds();

	Vector3 GetCentre() const { return (min + max) / 2.f; }
	Vector3 GetExtent() const { return (max - min) / 2.f; }
};
