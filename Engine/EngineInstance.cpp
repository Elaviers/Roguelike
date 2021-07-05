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
#include <ELSys/TextManager.hpp>
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
	delete pTextManager;
	delete pTextureManager;

	FT_Done_FreeType(_ftLib);
}

template <typename T>
inline T* CREATE(EEngineCreateFlags flags, EEngineCreateFlags cflag, Context& ctx)
{
	if ((flags & cflag) != (EEngineCreateFlags)0)
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
	
	_InitRegisters();
	context.Set(&objectTypes);
	context.Set(&geometryTypes);

	pConsole =			CREATE<Console>(flags, EEngineCreateFlags::CONSOLE, context);
	pAnimationManager = CREATE<AnimationManager>(flags, EEngineCreateFlags::ANIMATIONMGR, context);
	pAudioManager =		CREATE<AudioManager>(flags, EEngineCreateFlags::AUDIOMGR, context);
	pDebugManager =		CREATE<DebugManager>(flags, EEngineCreateFlags::DEBUGMGR, context);
	pFontManager =		CREATE<FontManager>(flags, EEngineCreateFlags::FONTMGR, context);
	pInputManager =		CREATE<InputManager>(flags, EEngineCreateFlags::INPUTMGR, context);
	pMaterialManager =	CREATE<MaterialManager>(flags, EEngineCreateFlags::MATERIALMGR, context);
	pMeshManager =		CREATE<MeshManager>(flags, EEngineCreateFlags::MESHMGR, context);
	pModelManager =		CREATE<ModelManager>(flags, EEngineCreateFlags::MODELMGR, context);
	pTextManager =		CREATE<TextManager>(flags, EEngineCreateFlags::TEXTMGR, context);
	pTextureManager =	CREATE<TextureManager>(flags, EEngineCreateFlags::TEXTUREMGR, context);
	pTileManager =		CREATE<TileManager>(flags, EEngineCreateFlags::TILEMGR, context);

	if (pConsole)
	{
		//Commands
		pConsole->Cvars().CreateVar("TexMgr", CommandPtr(&Console::CMD_texmgr, *pConsole));
		pConsole->Cvars().CreateVar("Play", CommandPtr(&AudioManager::CMD_play, *pAudioManager));
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

	if (pTextManager)
	{
		pTextManager->Initialise();
		pTextManager->SetRootPath("Data/Text/");

		pTextProvider = pTextManager->Get("default", context).Ptr();
		if (pTextProvider == nullptr) pTextProvider = pTextManager->Get("en_uk", context).Ptr();
		if (pTextProvider == nullptr) pTextProvider = pTextManager->Get("en", context).Ptr();
		if (pTextProvider == nullptr) pTextProvider = pTextManager->Get("core", context).Ptr();
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

#include "World.hpp"
#include "WorldObject.hpp"
#include "ORenderable.hpp"
#include "OSkeletal.hpp"
#include "OBrush3D.hpp"
#include "OBrush2D.hpp"
#include "OLight.hpp"
#include "OSprite.hpp"
#include "OConnector.hpp"
#include "OCamera.hpp"
#include "OGeometryCollection.hpp"
#include "GeoIsoTile.hpp"

#include "Geometry.hpp"
#include "GeoIsoTile.hpp"

template <typename T>
ObjectType MakeObjectType(const String& name) 
{ 
	return ObjectType(T::TypeID, name, MemberFunction<World, WorldObject*>((WorldObject* (World::*)())&World::CreateObject<T>)); 
}

template <typename T>
GeometryType MakeGeometryType(const String& name)
{
	return GeometryType(T::TypeID, name, MemberFunction<OGeometryCollection, Geometry*>((Geometry * (OGeometryCollection::*)()) & OGeometryCollection::CreateGeometry<T>));
}

void EngineInstance::_InitRegisters()
{
	objectTypes.RegisterType(MakeObjectType<WorldObject>("WorldObject"));
	objectTypes.RegisterType(MakeObjectType<ORenderable>("Renderable"));
	objectTypes.RegisterType(MakeObjectType<OBrush3D>("Brush"));
	objectTypes.RegisterType(MakeObjectType<OBrush2D>("Plane"));
	objectTypes.RegisterType(MakeObjectType<OLight>("Light"));
	objectTypes.RegisterType(MakeObjectType<OSprite>("Sprite"));
	objectTypes.RegisterType(MakeObjectType<OConnector>("Level Connector"));
	objectTypes.RegisterType(MakeObjectType<OCamera>("Camera"));
	objectTypes.RegisterType(MakeObjectType<OGeometryCollection>("Geometry"));

	geometryTypes.RegisterType(MakeGeometryType<GeoIsoTile>("Iso Tile"));
}
