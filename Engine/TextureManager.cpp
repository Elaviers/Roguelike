#include "TextureManager.h"
#include "Error.h"
#include "GL.h"
#include "IO.h"
#include <vector>

TextureManager::TextureManager()
{
}


TextureManager::~TextureManager()
{
	_colours.white.Delete();
	_colours.grey.Delete();
	_colours.uvDefault.Delete();

	_textures.ForEach(
		[](const String&, GLTexture &tex)
		{
			tex.Delete();
		});
}

void TextureManager::Initialise()
{
	byte white[4] = { 255, 255, 255, 255 };
	byte grey[4] = { 127, 127, 127, 255 };
	byte uvDefault[4] = {127, 127, 255, 255};

	_colours.white.Create(1, 1, white, GL_NEAREST);
	_colours.grey.Create(1, 1, grey, GL_NEAREST);
	_colours.uvDefault.Create(1, 1, uvDefault, GL_NEAREST);
}

const GLTexture* TextureManager::GetTexture(const char *name)
{
	GLTexture *tex = _textures.Find(name);
	if (tex) return tex;
	else
	{
		TextureData data = IO::ReadPNGFile((_rootPath + name).GetData());

		if (data.IsValid())
		{
			GLTexture &newTex = _textures[name];
			newTex.Create(data.width, data.height, data.data.Data());
			return &newTex;
		}
	}

	return nullptr;
}
