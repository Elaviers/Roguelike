#pragma once
#include "Vector.hpp"

struct Box
{
	Vector3 centre;
	Vector3 halfSize;

	Box(const Vector3& centre = Vector3(), const Vector3& extent = Vector3()) : centre(centre), halfSize(extent) {}

	static Box FromMinMax(const Vector3& min, const Vector3& max)
	{
		return Box((max + min) / 2.f, (max - min) / 2.f);
	}
};
