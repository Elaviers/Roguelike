#pragma once
#include "InputManager.h"
#include "MaterialManager.h"
#include "ModelManager.h"
#include "Registry.h"

namespace Engine
{
	namespace ObjectIDs
	{
		enum
		{
			NOT_OBJECT = 0,	//Used in level format for level messages
			RENDERABLE = 1,
			BRUSH2D = 2,
			BRUSH3D = 3,
			LIGHT = 4
		};
	}

	extern Registry registry;

	extern InputManager *inputManager;
	extern MaterialManager *materialManager;
	extern ModelManager *modelManager;
}
