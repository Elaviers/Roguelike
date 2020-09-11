#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Geometry.hpp"
#include "Registry.hpp"
#include <ELCore/Context.hpp>
#include <ELCore/Tracker.hpp>

class Console;
class AnimationManager;
class AudioManager;
class DebugManager;
class FontManager;
class InputManager;
class MaterialManager;
class MeshManager;
class ModelManager;
class TextureManager;
class TileManager;

enum class EEngineCreateFlags : uint16
{
	CONSOLE = 0x1,
	ANIMATIONMGR = 0x2,
	AUDIOMGR = 0x4,
	DEBUGMGR = 0x8,
	FONTMGR = 0x10,
	INPUTMGR = 0x20,
	MATERIALMGR = 0x40,
	MESHMGR = 0x80,
	MODELMGR = 0x100,
	TEXTUREMGR = 0x200,
	TILEMGR = 0x400,

	OBJTRACKER = 0x8000,

	ALL = 0xFFFF
};

#include <ELCore/MacroUtilities.hpp>
DEFINE_BITMASK_FUNCS(EEngineCreateFlags, uint16)

class EngineInstance
{
private:
	FT_Library _ftLib;

public:
	Registry registry;

	Context context;

	Console			*pConsole = nullptr;

	AnimationManager*pAnimationManager = nullptr;
	AudioManager	*pAudioManager = nullptr;
	DebugManager	*pDebugManager = nullptr;
	FontManager		*pFontManager = nullptr;
	InputManager	*pInputManager = nullptr;
	MaterialManager	*pMaterialManager = nullptr;
	MeshManager		*pMeshManager = nullptr;
	ModelManager	*pModelManager = nullptr;
	TextureManager	*pTextureManager = nullptr;
	TileManager		*pTileManager = nullptr;

	Tracker<Entity> *pObjectTracker = nullptr;

	EngineInstance() : _ftLib(0) {}
	~EngineInstance();

	FT_Library GetFTLibrary() { return _ftLib; }

	void Init(EEngineCreateFlags);
};
