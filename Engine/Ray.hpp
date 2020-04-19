#pragma once
#include "CollisionChannels.hpp"
#include "Vector3.hpp"

struct Ray
{
	Vector3 origin;
	Vector3 direction;

	ECollisionChannels channels;

	Ray() : channels(ECollisionChannels::ALL) {}
	Ray(const Vector3& origin, const Vector3& direction, ECollisionChannels channels) : origin(origin), direction(direction), channels(channels) {}
};
