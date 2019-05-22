#pragma once
#include "ResourceManager.hpp"
#include "GLTexture.hpp"

class TextureManager : public ResourceManager<GLTexture, true>
{
private:
	struct
	{
		GLTexture white, grey, uvDefault;
	} _colours;

	virtual bool _CreateResource(GLTexture&, const String&, const String&);

	virtual void _DestroyResource(GLTexture&);

	struct
	{
		int mipLevels = 8;
		GLint magFilter = GL_LINEAR;

	} _nextTextureInfo;

public:
	TextureManager() {}
	virtual ~TextureManager();

	void Initialise();

	void CMD_mag(const Buffer<String>& args);
	void CMD_mips(const Buffer<String>& args);

	inline void SetNextMipLevels(int levels) { _nextTextureInfo.mipLevels = levels; }
	inline void SetMagFilter(GLint mag) { _nextTextureInfo.magFilter = mag; }

	inline const GLTexture& White() { return _colours.white; }
	inline const GLTexture& Grey() { return _colours.grey; }
	inline const GLTexture& UVDefault() { return _colours.uvDefault; }
};
