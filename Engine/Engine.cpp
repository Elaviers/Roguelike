#include "Engine.hpp"
#include "Console.hpp"
#include "AnimationManager.hpp"
#include "AudioManager.hpp"
#include "DebugManager.hpp"
#include "FontManager.hpp"
#include "InputManager.hpp"
#include "MaterialManager.hpp"
#include "ModelManager.hpp"
#include "TextureManager.hpp"

Engine::~Engine()
{
	delete pConsole; 
	delete pAnimationManager; 
	delete pAudioManager;
	delete pDebugManager;
	delete pFontManager; 
	delete pInputManager;
	delete pMaterialManager;
	delete pModelManager;
	delete pTextureManager;
	delete pObjectTracker;

	FT_Done_FreeType(_ftLib);
}

#define CREATE(CONDITION, PTR) (CONDITION) ? (PTR) : nullptr 

void Engine::Init(EEngineCreateFlags flags, Entity *world)
{
	pWorld = world;

	FT_Error error = FT_Init_FreeType(&_ftLib);
	if (error) { Debug::Error("Freetype init error"); }
	
	registry.RegisterEngineObjects();

	pConsole =			CREATE(flags & EEngineCreateFlags::CONSOLE,		new Console());
	pAnimationManager = CREATE(flags & EEngineCreateFlags::ANIMATIONMGR,	new AnimationManager());
	pAudioManager =		CREATE(flags & EEngineCreateFlags::AUDIOMGR,		new AudioManager());
	pDebugManager =		CREATE(flags & EEngineCreateFlags::DEBUGMGR,		new DebugManager());
	pFontManager =		CREATE(flags & EEngineCreateFlags::FONTMGR,		new FontManager());
	pInputManager =		CREATE(flags & EEngineCreateFlags::INPUTMGR,		new InputManager());
	pMaterialManager =	CREATE(flags & EEngineCreateFlags::MATERIALMGR,	new MaterialManager());
	pModelManager =		CREATE(flags & EEngineCreateFlags::MODELMGR,		new ModelManager());
	pTextureManager =	CREATE(flags & EEngineCreateFlags::TEXTUREMGR,	new TextureManager());
	pObjectTracker =	CREATE(flags & EEngineCreateFlags::OBJTRACKER,	new Tracker<Entity>());

	if (pConsole)
	{
		//Commands
		pConsole->Cvars().CreateVar("TexMgr", CommandPtr(pTextureManager, &TextureManager::CMD_config));
		pConsole->Cvars().CreateVar("Play", CommandPtr(pAudioManager, &AudioManager::CMD_play));

		if (pWorld)
		{
			pConsole->Cvars().CreateVar("Ents", CommandPtr(pWorld, &Entity::CMD_List));
			pConsole->Cvars().CreateVar("Ent", CommandPtr(pWorld, &Entity::CMD_Ent));
		}
	}

	if (pAnimationManager)
	{
		pAnimationManager->Initialise();
		pAnimationManager->SetRootPath("Data/Animations/");
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

	if (pFontManager) 
	{
		pFontManager->Initialise();
		pFontManager->SetRootPath("Data/Fonts/");
	}
}
