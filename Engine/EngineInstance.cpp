#include "EngineInstance.hpp"
#include "Console.hpp"
#include "ModelManager.hpp"
#include "TileManager.hpp"
#include <ELGraphics/AnimationManager.hpp>
#include <ELAudio/AudioManager.hpp>
#include <ELGraphics/DebugManager.hpp>
#include <ELGraphics/FontManager.hpp>
#include <ELSys/InputManager.hpp>
#include <ELGraphics/MaterialManager.hpp>
#include <ELGraphics/MeshManager.hpp>
#include <ELGraphics/TextureManager.hpp>

EngineInstance::~EngineInstance()
{
	delete pConsole; 
	delete pAnimationManager; 
	delete pAudioManager;
	delete pDebugManager;
	delete pFontManager; 
	delete pInputManager;
	delete pMaterialManager;
	delete pMeshManager;
	delete pTextureManager;
	delete pObjectTracker;

	FT_Done_FreeType(_ftLib);
}

template <typename T>
inline T* CREATE(bool condition, Context& ctx)
{
	if (condition)
	{
		T* ptr = new T();
		ctx.Set(ptr);
		return ptr;
	}

	return nullptr;
}

void EngineInstance::Init(EEngineCreateFlags flags)
{
	FT_Error error = FT_Init_FreeType(&_ftLib);
	if (error) { Debug::Error("Freetype init error"); }

	context.Set(&_ftLib);
	
	_InitRegistries();
	context.Set(&entRegistry);
	context.Set(&geometryRegistry);

	pConsole =			CREATE<Console>(flags & EEngineCreateFlags::CONSOLE, context);
	pAnimationManager = CREATE<AnimationManager>(flags & EEngineCreateFlags::ANIMATIONMGR, context);
	pAudioManager =		CREATE<AudioManager>(flags & EEngineCreateFlags::AUDIOMGR, context);
	pDebugManager =		CREATE<DebugManager>(flags & EEngineCreateFlags::DEBUGMGR, context);
	pFontManager =		CREATE<FontManager>(flags & EEngineCreateFlags::FONTMGR, context);
	pInputManager =		CREATE<InputManager>(flags & EEngineCreateFlags::INPUTMGR, context);
	pMaterialManager =	CREATE<MaterialManager>(flags & EEngineCreateFlags::MATERIALMGR, context);
	pMeshManager =		CREATE<MeshManager>(flags & EEngineCreateFlags::MESHMGR, context);
	pModelManager =		CREATE<ModelManager>(flags & EEngineCreateFlags::MODELMGR, context);
	pTextureManager =	CREATE<TextureManager>(flags & EEngineCreateFlags::TEXTUREMGR, context);
	pTileManager =		CREATE<TileManager>(flags & EEngineCreateFlags::TILEMGR, context);
	pObjectTracker =	CREATE<Tracker<Entity>>(flags & EEngineCreateFlags::OBJTRACKER, context);

	if (pConsole)
	{
		//Commands
		pConsole->Cvars().CreateVar("TexMgr", CommandPtr(pConsole, &Console::CMD_texmgr));
		pConsole->Cvars().CreateVar("Play", CommandPtr(pAudioManager, &AudioManager::CMD_play));

		/* todo: move this somewhere else
		if (pWorld)
		{
			pConsole->Cvars().CreateVar("Ents", CommandPtr(pWorld, &Entity::CMD_List));
			pConsole->Cvars().CreateVar("Ent", CommandPtr(pWorld, &Entity::CMD_Ent));
		}
		*/
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

	if (pMeshManager)
	{
		pMeshManager->Initialise();
		pMeshManager->SetRootPath("Data/Meshes/");
	
		if (pModelManager)
		{
			pModelManager->Initialise(*pMeshManager);
			pModelManager->SetRootPath("Data/Models/");
		}
	}

	if (pTextureManager)
	{
		pTextureManager->Initialise();
		pTextureManager->SetRootPath("Data/Textures/");

		if (pMaterialManager)
		{
			pMaterialManager->Initialise(*pTextureManager);
			pMaterialManager->SetRootPath("Data/Materials/");
		}
	}

	if (pTileManager)
	{
		pTileManager->Initialise();
		pTileManager->SetRootPath("Data/Tiles/");
	}

	if (pFontManager) 
	{
		pFontManager->Initialise();
		pFontManager->SetRootPath("Data/Fonts/");
	}
}

#include "Entity.hpp"
#include "EntRenderable.hpp"
#include "EntSkeletal.hpp"
#include "EntBrush3D.hpp"
#include "EntBrush2D.hpp"
#include "EntLight.hpp"
#include "EntSprite.hpp"
#include "EntConnector.hpp"
#include "EntCamera.hpp"
#include "GeoIsoTile.hpp"

void EngineInstance::_InitRegistries()
{
	entRegistry.RegisterObjectClass<Entity>("Entity");
	entRegistry.RegisterObjectClass<EntRenderable>("Renderable");
	entRegistry.RegisterObjectClass<EntSkeletal>("Skeletal");
	entRegistry.RegisterObjectClass<EntBrush3D>("Brush");
	entRegistry.RegisterObjectClass<EntBrush2D>("Plane");
	entRegistry.RegisterObjectClass<EntLight>("Light");
	entRegistry.RegisterObjectClass<EntSprite>("Sprite");
	entRegistry.RegisterObjectClass<EntConnector>("Level Connector");
	entRegistry.RegisterObjectClass<EntCamera>("Camera");

	geometryRegistry.RegisterObjectClass<GeoIsoTile>("Iso Tile");
}
