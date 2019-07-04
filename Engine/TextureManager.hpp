#pragma once
#include "AssetManager.hpp"
#include "Texture.hpp"

class TextureManager : public AssetManager<Texture>
{
private:
	struct _TextureManagerColours
	{
		Texture white, grey, uvDefault;

		_TextureManagerColours() :
			white(Buffer<byte>({ 255, 255, 255, 255 }), 1, 1),
			grey(Buffer<byte>({ 127, 127, 127, 255 }), 1, 1),
			uvDefault(Buffer<byte>({ 127, 127, 255, 255 }), 1, 1) {}
	} _colours;

	virtual Texture* _CreateResource(const String&, const Buffer<byte>&);

	struct
	{
		int mipLevels = 8;
		GLint magFilter = GL_LINEAR;

	} _nextTextureInfo;

public:
	TextureManager() : AssetManager(".tex", "") {}
	
	virtual ~TextureManager()
	{
		_DestroyResource(_colours.white);
		_DestroyResource(_colours.grey);
		_DestroyResource(_colours.uvDefault);
	}

	void Initialise();

	void CMD_mag(const Buffer<String>& args);
	void CMD_mips(const Buffer<String>& args);

	inline void SetNextMipLevels(int levels) { _nextTextureInfo.mipLevels = levels; }
	inline void SetMagFilter(GLint mag) { _nextTextureInfo.magFilter = mag; }

	inline const Texture& White() { return _colours.white; }
	inline const Texture& Grey() { return _colours.grey; }
	inline const Texture& UVDefault() { return _colours.uvDefault; }
};
