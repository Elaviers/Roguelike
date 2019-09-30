#include "Engine.hpp"
#include "Console.hpp"
#include "AudioManager.hpp"
#include "DebugManager.hpp"
#include "FontManager.hpp"
#include "InputManager.hpp"
#include "MaterialManager.hpp"
#include "ModelManager.hpp"
#include "TextureManager.hpp"

Engine::~Engine()
{
	delete pConsole; delete pAudioManager; delete pDebugManager; delete pFontManager; delete pInputManager; delete pMaterialManager; delete pModelManager; delete pTextureManager;

	FT_Done_FreeType(_ftLib);
}

#define CREATE(CONDITION, PTR) (CONDITION) ? (PTR) : nullptr 

void Engine::Init(EngineCreateFlags flags)
{
	FT_Error error = FT_Init_FreeType(&_ftLib);
	if (error) { Debug::Error("Freetype init error"); }
	
	registry.RegisterEngineObjects();

	pConsole =			CREATE(flags & ENG_CONSOLE,		new Console());
	pAudioManager =		CREATE(flags & ENG_AUDIOMGR,	new AudioManager());
	pDebugManager =		CREATE(flags & ENG_DBGMGR,		new DebugManager());
	pFontManager =		CREATE(flags & ENG_FONTMGR,		new FontManager());
	pInputManager =		CREATE(flags & ENG_INPUTMGR,	new InputManager());
	pMaterialManager =	CREATE(flags & ENG_MATERIALMGR, new MaterialManager());
	pModelManager =		CREATE(flags & ENG_MODELMGR,	new ModelManager());
	pTextureManager =	CREATE(flags & ENG_TEXTUREMGR,	new TextureManager());
	pInputManager =		CREATE(flags & ENG_INPUTMGR,	new InputManager());
	pObjectTracker =	CREATE(flags & ENG_OBJTRACKER,	new Tracker<GameObject>());

	if (pConsole)
	{
		//Commands
		pConsole->Cvars().CreateVar("play", CommandPtr(this->pAudioManager, &AudioManager::CMD_play));
	}

	if (pAudioManager)
	{
		pAudioManager->Initialise();
		pAudioManager->SetRootPath("Data/Audio/");
	}

	if (pModelManager)
	{
		pModelManager->Initialise();
		pModelManager->SetRootPath("Data/Models/");
	}

	if (pTextureManager)
	{
		pTextureManager->Initialise();
		pTextureManager->SetRootPath("Data/Textures/");
	}

	if (pMaterialManager)
	{
		pMaterialManager->Initialise();
		pMaterialManager->SetRootPath("Data/Materials/");
	}

	if (pFontManager) pFontManager->SetRootPath("Data/Fonts/");
}
