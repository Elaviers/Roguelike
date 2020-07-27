#pragma once
#include <ELCore/Asset.hpp>
#include <ELCore/Buffer.hpp>
#include <ELMaths/Maths.hpp>
#include <ELSys/GLTexture.hpp>

class Texture : public Asset
{
public:
	struct Info
	{
		byte aniso;
		byte mipLevels;
		GLint minFilter;
		GLint magFilter;

		void CMD_min(const Buffer<String>& args, const Context&);
		void CMD_mag(const Buffer<String>& args, const Context&);

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
	uint32 width;
	uint32 _height;

	GLTexture _glTexture;

	void _CMD_img(const Buffer<String>& args, const Context&);

public:
	Info info;

	Texture() : width(0), _height(0) {}
	Texture(const Buffer<byte> &data, uint32 width, uint32 height) : _data(data), width(width), _height(height) {}
	Texture(Buffer<byte> &&data, uint32 width, uint32 height) : _data(data), width(width), _height(height) {}
	virtual ~Texture() { _glTexture.Delete(); }

	const GLTexture& GL() const { return _glTexture; }

	virtual const PropertyCollection& GetProperties() override;

	bool IsValid() const { return _data.GetSize() != 0; }

	const Buffer<byte>& GetData() const { return _data; }
	uint32 GetWidth() const { return width; }
	uint32 GetHeight() const { return _height; }

	void Create(byte maxMipLevels = 0, byte maxAnisotropy = 0) 
	{
		_glTexture.Delete();
		_glTexture.Create(width, _height, _data.Data(), 
			maxMipLevels ? Maths::Min(maxMipLevels, info.mipLevels) : info.mipLevels, 
			maxAnisotropy ? Maths::Min(maxAnisotropy, info.aniso) : info.aniso, 
			info.minFilter, 
			info.magFilter);
	}

	void Bind(int unit) const
	{
		_glTexture.Bind(unit);
	}
};
