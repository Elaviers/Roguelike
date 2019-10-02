#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Registry.hpp"
#include "Tracker.hpp"

class Console;
class GameObject;
class AudioManager;
class DebugManager;
class FontManager;
class InputManager;
class MaterialManager;
class ModelManager;
class TextureManager;

enum EngineCreateFlags
{
	ENG_CONSOLE = 0x1,
	ENG_AUDIOMGR = 0x2,
	ENG_DBGMGR = 0x4,
	ENG_FONTMGR = 0x8,
	ENG_INPUTMGR = 0x10,
	ENG_MATERIALMGR = 0x20,
	ENG_MODELMGR = 0x40,
	ENG_TEXTUREMGR = 0x80,
	ENG_OBJTRACKER = 0x8000,

	ENG_ALL = 0xFFFF
};

class Engine
{
private:
	Engine() : _ftLib(0) {}
	~Engine();

	FT_Library _ftLib;

public:
	inline static Engine& Instance() { static Engine _instance;  return _instance; }

	Registry registry;

	Console			*pConsole = nullptr;
	GameObject		*pWorld = nullptr;
	
	AudioManager	*pAudioManager = nullptr;
	DebugManager	*pDebugManager = nullptr;
	FontManager		*pFontManager = nullptr;
	InputManager	*pInputManager = nullptr;
	MaterialManager	*pMaterialManager = nullptr;
	ModelManager	*pModelManager = nullptr;
	TextureManager	*pTextureManager = nullptr;

	Tracker<GameObject> *pObjectTracker = nullptr;

	inline FT_Library GetFTLibrary() { return _ftLib; }

	void Init(EngineCreateFlags, GameObject* world);
};
