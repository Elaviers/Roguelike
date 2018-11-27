#pragma once
#include "FontManager.h"
#include "InputManager.h"
#include "MaterialManager.h"
#include "ModelManager.h"
#include "TextureManager.h"
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

	extern FontManager *fontManager;
	extern InputManager *inputManager;
	extern MaterialManager *materialManager;
	extern ModelManager *modelManager;
	extern TextureManager *textureManager;
}
