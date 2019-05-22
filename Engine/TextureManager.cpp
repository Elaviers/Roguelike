#include "TextureManager.hpp"
#include "Debug.hpp"
#include "GL.hpp"
#include "IO.hpp"
#include <vector>


TextureManager::~TextureManager()
{
	_colours.white.Delete();
	_colours.grey.Delete();
	_colours.uvDefault.Delete();
}

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

bool TextureManager::_CreateResource(GLTexture& tex, const String& name, const String& filepath)
{
	TextureData data = IO::ReadPNGFile(filepath.GetData());

	bool success = false;

	if (data.IsValid())
	{
		tex.Create(data.width, data.height, data.data.Data(), _nextTextureInfo.mipLevels, _nextTextureInfo.magFilter);
		success	= true;
	}

	_nextTextureInfo.mipLevels = 8;
	_nextTextureInfo.magFilter = GL_LINEAR;

	return success;
}

void TextureManager::_DestroyResource(GLTexture& tex)
{
	tex.Delete();
}

void TextureManager::Initialise()
{
	byte white[4] = { 255, 255, 255, 255 };
	byte grey[4] = { 127, 127, 127, 255 };
	byte uvDefault[4] = {127, 127, 255, 255};

	_colours.white.Create(1, 1, white, 1, GL_NEAREST);
	_colours.grey.Create(1, 1, grey, 1, GL_NEAREST);
	_colours.uvDefault.Create(1, 1, uvDefault, 1, GL_NEAREST);
}
