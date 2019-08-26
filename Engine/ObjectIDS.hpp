#pragma once

namespace ObjectIDS
{
	enum ObjectID : byte
	{
		GAMEOBJECT = 0xFF,
		RENDERABLE = 1,
		BRUSH = 2,
		PLANE = 3,
		LIGHT = 4,
		SPRITE = 5,
		LEVEL_CONNECTOR = 100,
		BOX = 101,
		CAMERA = 102
	};
}
