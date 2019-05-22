#pragma once
#include "Vector.hpp"

class GameObject;

struct RaycastResult
{
	GameObject *object;

	float entryTime;
};
