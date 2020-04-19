#include "TextureManager.hpp"
#include "Console.hpp"
#include "Debug.hpp"
#include "GL.hpp"
#include "IO.hpp"
#include "MaterialManager.hpp"
#include <vector>

constexpr const char* MAXANISOTROPY = "max_aniso";
constexpr const char* MAXMIPS = "max_mip_levels";

void TextureManager::CMD_config(const Buffer<String>& args)
{
	if (args.GetSize() >= 2)
	{
		String var = args[0].ToLower();

		if (var == MAXANISOTROPY)	
		{ 
			_maxAnisotropy = args[1].ToInt(); 
			UnloadAssets();
			Engine::Instance().pMaterialManager->UnloadAssets();
			return; 
		}
		
		if (var == MAXMIPS)	
		{ 
			_maxMipLevels = args[1].ToInt(); 
			UnloadAssets();
			Engine::Instance().pMaterialManager->UnloadAssets();
			return; 
		}
	}
	
	Engine::Instance().pConsole->Print(CSTR(MAXANISOTROPY, '=', (int)_maxAnisotropy, '\n'));
	Engine::Instance().pConsole->Print(CSTR(MAXMIPS, '=', (int)_maxMipLevels, '\n'));
}

Texture* TextureManager::_CreateResource(const String& name, const Buffer<byte> &data)
{
	Texture* tex = IO::ReadPNGFile(data);

	if (tex && tex->IsValid())
		tex->Create(_maxMipLevels, _maxAnisotropy);
	
	return tex;
}

Texture* TextureManager::_CreateResource(const String& name, const String& text)
{
	Texture* tex = Asset::FromText<Texture>(text);

	if (tex == nullptr)
		Debug::Error(CSTR("Could not load texture \"", name, '\"'));

	return tex;
}

void TextureManager::Initialise()
{
	_colours.black = new Texture(Buffer<byte>({ 0, 0, 0, 255 }), 1, 1);
	_colours.white = new Texture(Buffer<byte>({ 255, 255, 255, 255 }), 1, 1);
	_colours.grey = new Texture(Buffer<byte>({ 127, 127, 127, 255 }), 1, 1);
	_colours.normalDefault = new Texture(Buffer<byte>({ 127, 127, 255, 255 }), 1, 1);

	Texture::Info colourInfo;
	colourInfo.aniso = 1;
	colourInfo.mipLevels = 1;
	colourInfo.minFilter = colourInfo.magFilter = GL_NEAREST;

	_colours.black->info = colourInfo;
	_colours.white->info = colourInfo;
	_colours.grey->info = colourInfo;
	_colours.normalDefault->info = colourInfo;

	_colours.black->Create();
	_colours.white->Create();
	_colours.grey->Create();
	_colours.normalDefault->Create();

	SharedPointerData<Texture>& dBlack = _MapValue("black"), & dWhite = _MapValue("white"), & dGrey = _MapValue("grey"), & dNormalDefault = _MapValue("normal_default");

	dBlack.SetPtr(_colours.black);
	dWhite.SetPtr(_colours.white);
	dGrey.SetPtr(_colours.grey);
	dNormalDefault.SetPtr(_colours.normalDefault);

	_colours.tBlack = SharedPointer<Texture>(dBlack);
	_colours.tWhite = SharedPointer<Texture>(dWhite);
	_colours.tGrey = SharedPointer<Texture>(dGrey);
	_colours.tNormalDefault = SharedPointer<Texture>(dNormalDefault);
}
