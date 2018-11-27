#pragma once
#include "Vector.h"

enum class RayChannel
{
	CAMERA,
	COLLISION
};

struct Ray
{
	Vector3 origin;
	Vector3 direction;

	RayChannel channel;

	Ray(Vector3 origin, Vector3 direction, RayChannel channel = RayChannel::COLLISION) : origin(origin), direction(direction), channel(channel) {}
};
