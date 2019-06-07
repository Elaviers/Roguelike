#include "TextureManager.hpp"
#include "Debug.hpp"
#include "GL.hpp"
#include "IO.hpp"
#include <vector>


TextureManager::~TextureManager()
{
	_DestroyResource(_colours.white);
	_DestroyResource(_colours.grey);
	_DestroyResource(_colours.uvDefault);
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

GLTexture* TextureManager::_CreateResource(const String& name, const String& filepath)
{
	GLTexture* tex = nullptr;

	TextureData data = IO::ReadPNGFile(filepath.GetData());
	if (data.IsValid())
	{
		tex = new GLTexture();
		tex->Create(data.width, data.height, data.data.Data(), _nextTextureInfo.mipLevels, _nextTextureInfo.magFilter);
	}

	_nextTextureInfo.mipLevels = 8;
	_nextTextureInfo.magFilter = GL_LINEAR;

	return tex;
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
