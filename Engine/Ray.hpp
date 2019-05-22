#pragma once
#include "Vector.hpp"

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
