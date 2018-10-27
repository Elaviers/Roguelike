#pragma once

class GameObject;

struct RaycastResult
{
	GameObject *object;

	float entryTime;
	float exitTime;
};
