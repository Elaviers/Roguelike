#pragma once
#include "CollisionChannel.hpp"
#include "Vector.hpp"

struct Ray
{
	Vector3 origin;
	Vector3 direction;

	CollisionChannels channels;

	Ray() : channels(CollisionChannels::ALL) {}
	Ray(Vector3 origin, Vector3 direction, CollisionChannels channels) : origin(origin), direction(direction), channels(channels) {}
};
