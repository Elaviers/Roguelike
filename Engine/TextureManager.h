#pragma once
#include "GLTexture.h"
#include "Map.h"
#include "String.h"

class TextureManager
{
private:
	Map<String, GLTexture> _textures;

	struct
	{
		GLTexture white, grey, uvDefault;
	} _colours;

public:
	TextureManager();
	~TextureManager();

	void Initialise();

	const GLTexture& GetTexture(const char *name);

	inline const GLTexture& White() { return _colours.white; }
	inline const GLTexture& Grey() { return _colours.grey; }
	inline const GLTexture& UVDefault() { return _colours.uvDefault; }
};
