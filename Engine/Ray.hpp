#pragma once
#include "CollisionChannel.hpp"
#include "Vector.hpp"

struct Ray
{
	Vector3 origin;
	Vector3 direction;

	CollisionChannel channels;

	Ray() : channels(COLL_ALL_CHANNELS) {}
	Ray(Vector3 origin, Vector3 direction, CollisionChannel channels) : origin(origin), direction(direction), channels(channels) {}
};
