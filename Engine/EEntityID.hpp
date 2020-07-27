#pragma once
#include <ELCore/Types.hpp>

enum class EEntityID : byte
{
	NONE = 0,

	//Saveable objects
	Entity = 0xFF,
	RENDERABLE = 1,
	SKELETAL = 2,
	BRUSH = 3,
	PLANE = 4,
	LIGHT = 5,
	SPRITE = 6,

	//Unsaveable objects
	LEVEL_CONNECTOR = 50,
	BOX = 51,
	CAMERA = 52
};
