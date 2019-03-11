#pragma once
#include "Registry.h"

class Console;
class AudioManager;
class DebugManager;
class FontManager;
class InputManager;
class MaterialManager;
class ModelManager;
class TextureManager;

class Engine
{
private:
	Engine() {}
	~Engine();

public:
	inline static Engine& Instance() { static Engine _instance;  return _instance; }

	Registry registry;

	Console			*pConsole = nullptr;
	AudioManager	*pAudioManager = nullptr;
	DebugManager	*pDebugManager = nullptr;
	FontManager		*pFontManager = nullptr;
	InputManager	*pInputManager = nullptr;
	MaterialManager	*pMaterialManager = nullptr;
	ModelManager	*pModelManager = nullptr;
	TextureManager	*pTextureManager = nullptr;

	//Allocates all managers with default setup
	void DefaultInit();
};
