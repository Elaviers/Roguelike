#pragma once
#include "Vector.hpp"

class Entity;

struct RaycastResult
{
	Entity *object;

	float entryTime;
};
