#include "Texture.hpp"
#include "Engine.hpp"
#include "MacroUtilities.hpp"
#include "TextureManager.hpp"

void Texture::Info::CMD_min(const Buffer<String>& args)
{
	if (args.GetSize() > 0)
	{
		String string = args[0].ToLower();

		if (string == "nearest")						magFilter = GL_NEAREST;
		else if (string == "nearest_mipmap_nearest")	magFilter = GL_NEAREST_MIPMAP_NEAREST;
		else if (string == "nearest_mipmap_linear")		magFilter = GL_NEAREST_MIPMAP_LINEAR;
		else if (string == "linear")					magFilter = GL_LINEAR;
		else if (string == "linear_mipmap_nearest")		magFilter = GL_LINEAR_MIPMAP_NEAREST;
		else if (string == "linear_mipmap_linear")		magFilter = GL_LINEAR_MIPMAP_LINEAR;
	}

}

void Texture::Info::CMD_mag(const Buffer<String>& args)
{
	if (args.GetSize() > 0)
	{
		String string = args[0].ToLower();

		if (string == "nearest")		magFilter = GL_NEAREST;
		else if (string == "linear")	magFilter = GL_LINEAR;
	}
}

void Texture::_CMD_img(const Buffer<String>& args)
{
	if (args.GetSize() > 0)
	{
		const Buffer<String> paths = Engine::Instance().pTextureManager->GetPaths();

		for (size_t i = 0; i < paths.GetSize(); ++i)
		{
			String filename = paths[paths.GetSize() - 1 - i] + args[0];

			if (IO::FileExists(filename.GetData()) && IO::ReadPNGFile(filename.GetData(), _data, _width, _height))
			{
				Create(Engine::Instance().pTextureManager->GetMaxMipLevels(), Engine::Instance().pTextureManager->GetMaxAnisotropy());
				break;
			}
		}
	}
}

const PropertyCollection& Texture::GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE_BEGIN;
	properties.Add<byte>("aniso", offsetof(Texture, info.aniso));
	properties.Add<byte>("mips", offsetof(Texture, info.mipLevels));
	properties.AddCommand("min", MemberCommandPtr<Texture::Info>(&Texture::Info::CMD_min), offsetof(Texture, info));
	properties.AddCommand("mag", MemberCommandPtr<Texture::Info>(&Texture::Info::CMD_mag), offsetof(Texture, info));
	properties.AddCommand("img", MemberCommandPtr<Texture>(&Texture::_CMD_img));
	DO_ONCE_END;

	return properties;
}