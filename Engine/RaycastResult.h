#pragma once
#include "Vector.h"

class GameObject;

struct RaycastResult
{
	GameObject *object;

	float entryTime;
};
