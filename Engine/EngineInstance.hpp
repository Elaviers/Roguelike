#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Geometry.hpp"
#include "GeometryType.hpp"
#include "ObjectType.hpp"
#include <ELCore/Context.hpp>

class Console;
class AnimationManager;
class AudioManager;
class DebugManager;
class FontManager;
class InputManager;
class MaterialManager;
class MeshManager;
class ModelManager;
class TextManager;
class TextProvider;
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
	TEXTMGR = 0x200,
	TEXTUREMGR = 0x400,
	TILEMGR = 0x800,

	OBJTRACKER = 0x8000,

	ALL = 0xFFFF
};

#include "WorldObject.hpp"
#include <ELCore/MacroUtilities.hpp>
DEFINE_BITMASK_FUNCS(EEngineCreateFlags, uint16)

class EngineInstance
{
private:
	FT_Library _ftLib;

	void _InitRegisters();

public:
	ObjectRegister objectTypes;
	GeometryRegister geometryTypes;

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
	TextManager		*pTextManager = nullptr;
	TextProvider	*pTextProvider = nullptr;
	TextureManager	*pTextureManager = nullptr;
	TileManager		*pTileManager = nullptr;

	EngineInstance() : _ftLib(0) {}
	~EngineInstance();

	FT_Library GetFTLibrary() { return _ftLib; }

	void Init(EEngineCreateFlags);
};
