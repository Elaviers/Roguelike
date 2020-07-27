#include "TextureManager.hpp"
#include "IO.hpp"
#include "MaterialManager.hpp"
#include <ELSys/Debug.hpp>
#include <ELSys/GL.hpp>
#include <vector>

Texture* TextureManager::_CreateResource(const Buffer<byte>& data, const String& name, const String& extension, const Context& ctx)
{
	Texture* tex = IO::ReadPNGFile(data);

	if (tex && tex->IsValid())
		tex->Create(_maxMipLevels, _maxAnisotropy);
	
	return tex;
}

Texture* TextureManager::_CreateResource(const String& textData, const String& name, const String& extension, const Context& ctx)
{
	Texture* tex = Asset::FromText<Texture>(textData, ctx);

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

	SharedPointerData<Texture>& dBlack = _MapValue("black"), & dWhite = _MapValue("white"), & dGrey = _MapValue("grey"), & dNormalDefault = _MapValue("unitnormal");

	dBlack.SetPtr(_colours.black);
	dWhite.SetPtr(_colours.white);
	dGrey.SetPtr(_colours.grey);
	dNormalDefault.SetPtr(_colours.normalDefault);

	_colours.tBlack = SharedPointer<Texture>(dBlack);
	_colours.tWhite = SharedPointer<Texture>(dWhite);
	_colours.tGrey = SharedPointer<Texture>(dGrey);
	_colours.tNormalDefault = SharedPointer<Texture>(dNormalDefault);
}
