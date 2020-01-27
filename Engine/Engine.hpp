#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Registry.hpp"
#include "Tracker.hpp"

class Console;
class Entity;
class AnimationManager;
class AudioManager;
class DebugManager;
class FontManager;
class InputManager;
class MaterialManager;
class ModelManager;
class TextureManager;

enum class EngineCreateFlags : uint16
{
	CONSOLE = 0x1,
	ANIMATIONMGR = 0x2,
	AUDIOMGR = 0x4,
	DEBUGMGR = 0x8,
	FONTMGR = 0x10,
	INPUTMGR = 0x20,
	MATERIALMGR = 0x40,
	MODELMGR = 0x80,
	TEXTUREMGR = 0x100,

	OBJTRACKER = 0x8000,

	ALL = 0xFFFF
};

#include "MacroUtilities.hpp"
DEFINE_BITMASK_FUNCS(EngineCreateFlags, uint16)

class Engine
{
private:
	Engine() : _ftLib(0) {}
	~Engine();

	FT_Library _ftLib;

public:
	static Engine& Instance() { static Engine _instance;  return _instance; }

	Registry registry;

	Console			*pConsole = nullptr;
	Entity			*pWorld = nullptr;
	
	AnimationManager*pAnimationManager = nullptr;
	AudioManager	*pAudioManager = nullptr;
	DebugManager	*pDebugManager = nullptr;
	FontManager		*pFontManager = nullptr;
	InputManager	*pInputManager = nullptr;
	MaterialManager	*pMaterialManager = nullptr;
	ModelManager	*pModelManager = nullptr;
	TextureManager	*pTextureManager = nullptr;

	Tracker<Entity> *pObjectTracker = nullptr;

	FT_Library GetFTLibrary() { return _ftLib; }

	void Init(EngineCreateFlags, Entity* world);
};
