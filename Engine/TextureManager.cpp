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
	_colours.white.Create(1, GL_NEAREST);
	_colours.grey.Create(1, GL_NEAREST);
	_colours.uvDefault.Create(1, GL_NEAREST);
}
