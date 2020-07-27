#include "Texture.hpp"
#include "IO.hpp"
#include "TextureManager.hpp"
#include <ELCore/Context.hpp>
#include <ELCore/MacroUtilities.hpp>

void Texture::Info::CMD_min(const Buffer<String>& args, const Context& ctx)
{
	if (args.GetSize() > 0)
	{
		String string = args[0].ToLower();

		if (string == "nearest")						minFilter = GL_NEAREST;
		else if (string == "nearest_mipmap_nearest")	minFilter = GL_NEAREST_MIPMAP_NEAREST;
		else if (string == "nearest_mipmap_linear")		minFilter = GL_NEAREST_MIPMAP_LINEAR;
		else if (string == "linear")					minFilter = GL_LINEAR;
		else if (string == "linear_mipmap_nearest")		minFilter = GL_LINEAR_MIPMAP_NEAREST;
		else if (string == "linear_mipmap_linear")		minFilter = GL_LINEAR_MIPMAP_LINEAR;
	}

}

void Texture::Info::CMD_mag(const Buffer<String>& args, const Context& ctx)
{
	if (args.GetSize() > 0)
	{
		String string = args[0].ToLower();

		if (string == "nearest")		magFilter = GL_NEAREST;
		else if (string == "linear")	magFilter = GL_LINEAR;
	}
}

void Texture::_CMD_img(const Buffer<String>& args, const Context& ctx)
{
	if (args.GetSize() > 0)
	{
		TextureManager* textureManager = ctx.GetPtr<TextureManager>();
		if (textureManager)
		{
			const Buffer<String> paths = textureManager->GetPaths();

			for (size_t i = 0; i < paths.GetSize(); ++i)
			{
				String filename = paths[paths.GetSize() - 1 - i] + args[0];

				if (IO::FileExists(filename.GetData()) && IO::ReadPNGFile(filename.GetData(), _data, width, _height))
				{
					Create(textureManager->GetMaxMipLevels(), textureManager->GetMaxAnisotropy());
					break;
				}
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
