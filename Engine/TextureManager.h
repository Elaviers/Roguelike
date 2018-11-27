#pragma once
#include "ResourceManager.h"
#include "GLTexture.h"

class TextureManager : public ResourceManager<GLTexture>
{
private:
	struct
	{
		GLTexture white, grey, uvDefault;
	} _colours;

public:
	TextureManager();
	virtual ~TextureManager();

	void Initialise();

	const GLTexture* GetTexture(const String &name);

	inline const GLTexture& White() { return _colours.white; }
	inline const GLTexture& Grey() { return _colours.grey; }
	inline const GLTexture& UVDefault() { return _colours.uvDefault; }
};
