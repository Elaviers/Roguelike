#include "Material_Sprite.hpp"
#include "TextureManager.hpp"
#include <ELCore/Context.hpp>
#include <ELCore/MacroUtilities.hpp>

const PropertyCollection& MaterialSprite::GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE(properties.AddCommand("tex", MemberCommandPtr<MaterialSprite>(&MaterialSprite::CMD_texmgr)));

	return properties;
}

void MaterialSprite::CMD_texmgr(const Buffer<String>& args, const Context& ctx)
{
	if (args.GetSize() > 0)
	{
		TextureManager* textureManager = ctx.GetPtr<TextureManager>();
		if (textureManager)
			_diffuse = textureManager->Get(args[0], ctx);
	}
}
