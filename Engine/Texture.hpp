#pragma once
#include "Asset.hpp"
#include "Buffer.hpp"
#include "GLTexture.hpp"

class Texture : public Asset
{
	Buffer<byte> _data;
	uint32 _width;
	uint32 _height;

	GLTexture _glTexture;

public:
	Texture(const Buffer<byte> &data, uint32 width, uint32 height) : _data(data), _width(width), _height(height) {}
	Texture(Buffer<byte> &&data, uint32 width, uint32 height) : _data(data), _width(width), _height(height) {}
	virtual ~Texture() { _glTexture.Delete(); }

	inline bool IsValid() const { return _data.GetSize() != 0; }

	inline const Buffer<byte>& GetData() const { return _data; }
	inline uint32 GetWidth() const { return _width; }
	inline uint32 GetHeight() const { return _height; }

	inline void Create(int mipLevels, int glMagFilter) 
	{
		_glTexture.Delete();
		_glTexture.Create(_width, _height, _data.Data(), mipLevels, glMagFilter);
	}

	inline void Bind(int unit) const
	{
		_glTexture.Bind(unit);
	}
};
