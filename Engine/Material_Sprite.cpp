#include "Material_Sprite.hpp"
#include "MacroUtilities.hpp"

const PropertyCollection& MaterialSprite::GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE(properties.AddCommand("tex", MemberCommandPtr<MaterialSprite>(&MaterialSprite::_CMD_tex)));

	return properties;
}

void MaterialSprite::_CMD_tex(const Buffer<String>& args)
{
	if (args.GetSize() > 0)
	{
		_diffuse = Engine::Instance().pTextureManager->Get(args[0]);
	}
}
