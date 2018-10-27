#pragma once
#include "Vector.h"

struct Ray
{
	Vector3 origin;
	Vector3 direction;

	Ray(Vector3 origin, Vector3 direction) : origin(origin), direction(direction) {}
};
