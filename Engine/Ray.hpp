#pragma once
#include "CollisionChannels.hpp"
#include "Vector.hpp"

struct Ray
{
	Vector3 origin;
	Vector3 direction;

	ECollisionChannels channels;

	Ray() : channels(ECollisionChannels::ALL) {}
	Ray(Vector3 origin, Vector3 direction, ECollisionChannels channels) : origin(origin), direction(direction), channels(channels) {}
};
