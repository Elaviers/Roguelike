#pragma once
#include "Types.hpp"

namespace EntityIDS
{
	/*
		Entity IDs for Engine objects

	*/

	enum ObjectID : byte
	{
		//Saveable objects
		Entity = 0xFF,
		RENDERABLE = 1,
		SKELETAL = 2,
		BRUSH = 3,
		PLANE = 4,
		LIGHT = 5,
		SPRITE = 6,

		LEVEL_CONNECTOR = 50,
		BOX = 51,
		CAMERA = 52
	};
}
