#include "TextureManager.hpp"
#include "Debug.hpp"
#include "GL.hpp"
#include "IO.hpp"
#include <vector>

void TextureManager::CMD_mag(const Buffer<String>& args)
{
	if (args.GetSize() > 0)
	{
		if (args[0] == "nearest")
			_nextTextureInfo.magFilter = GL_NEAREST;
	}
}

void TextureManager::CMD_mips(const Buffer<String>& args)
{
	if (args.GetSize() > 0)
		_nextTextureInfo.mipLevels = args[0].ToInt();
}

Texture* TextureManager::_CreateResource(const String& name, const Buffer<byte> &data)
{
	Texture* tex = IO::ReadPNGTexture(data);

	if (tex && tex->IsValid())
	{
		tex->Create(_nextTextureInfo.mipLevels, _nextTextureInfo.magFilter);
	}

	_nextTextureInfo.mipLevels = 8;
	_nextTextureInfo.magFilter = GL_LINEAR;

	return tex;
}

void TextureManager::Initialise()
{
	_MapValue("white") = _colours.white = new Texture(Buffer<byte>({ 255, 255, 255, 255 }), 1, 1);
	_MapValue("gray") = _colours.grey = new Texture(Buffer<byte>({ 127, 127, 127, 255 }), 1, 1);
	_MapValue("normal_default") = _colours.normalDefault = new Texture(Buffer<byte>({ 127, 127, 255, 255 }), 1, 1);

	_colours.white->Create(1, GL_NEAREST);
	_colours.grey->Create(1, GL_NEAREST);
	_colours.normalDefault->Create(1, GL_NEAREST);
}
