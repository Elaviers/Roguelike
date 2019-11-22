#pragma once
#include "Asset.hpp"
#include "Buffer.hpp"
#include "GLTexture.hpp"

class Texture : public Asset
{
public:
	struct Info
	{
		byte aniso;
		byte mipLevels;
		GLint minFilter;
		GLint magFilter;

		void CMD_min(const Buffer<String>& args);
		void CMD_mag(const Buffer<String>& args);

		void SetDefault()
		{
			aniso = 16;
			mipLevels = 8;
			minFilter = GL_LINEAR_MIPMAP_LINEAR;
			magFilter = GL_NEAREST;
		}

		Info()
		{
			SetDefault();
		}
	};


private:
	Buffer<byte> _data;
	uint32 _width;
	uint32 _height;

	GLTexture _glTexture;

	void _CMD_img(const Buffer<String>& args);

public:
	Info info;

	Texture() : _width(0), _height(0) {}
	Texture(const Buffer<byte> &data, uint32 width, uint32 height) : _data(data), _width(width), _height(height) {}
	Texture(Buffer<byte> &&data, uint32 width, uint32 height) : _data(data), _width(width), _height(height) {}
	virtual ~Texture() { _glTexture.Delete(); }

	virtual const PropertyCollection& GetProperties() override;

	bool IsValid() const { return _data.GetSize() != 0; }

	const Buffer<byte>& GetData() const { return _data; }
	uint32 GetWidth() const { return _width; }
	uint32 GetHeight() const { return _height; }

	void Create(byte maxMipLevels = 0, byte maxAnisotropy = 0) 
	{
		_glTexture.Delete();
		_glTexture.Create(_width, _height, _data.Data(), 
			maxMipLevels ? Utilities::Min(maxMipLevels, info.mipLevels) : info.mipLevels, 
			maxAnisotropy ? Utilities::Min(maxAnisotropy, info.aniso) : info.aniso, 
			info.minFilter, 
			info.magFilter);
	}

	void Bind(int unit) const
	{
		_glTexture.Bind(unit);
	}
};
