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

void Engine::Init()
{
	FT_Error error = FT_Init_FreeType(&_ftLib);
	if (error) { Debug::Error("Freetype init error"); }
}

void Engine::CreateAllManagers()
{
	this->registry.RegisterEngineObjects();

	this->pConsole = new Console();
	this->pAudioManager = new AudioManager();
	this->pDebugManager = new DebugManager();
	this->pFontManager = new FontManager();
	this->pInputManager = new InputManager();
	this->pMaterialManager = new MaterialManager();
	this->pModelManager = new ModelManager();
	this->pTextureManager = new TextureManager();

	this->pConsole->Initialise();
	this->pAudioManager->Initialise();
	this->pModelManager->Initialise();
	this->pTextureManager->Initialise();

	this->pAudioManager->SetRootPath("Data/Audio/");
	this->pFontManager->SetRootPath("Data/Fonts/");
	this->pMaterialManager->SetRootPath("Data/Materials/");
	this->pModelManager->SetRootPath("Data/Models/");
	this->pTextureManager->SetRootPath("Data/Textures/");

	//Commands
	this->pConsole->Cvars().CreateVar("play", CommandPtr(this->pAudioManager, &AudioManager::CMD_play));
}
