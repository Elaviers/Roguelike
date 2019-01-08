#pragma once
#include "FontManager.h"
#include "InputManager.h"
#include "MaterialManager.h"
#include "ModelManager.h"
#include "TextureManager.h"
#include "Registry.h"

namespace Engine
{
	extern Registry registry;

	extern FontManager *fontManager;
	extern InputManager *inputManager;
	extern MaterialManager *materialManager;
	extern ModelManager *modelManager;
	extern TextureManager *textureManager;
}
