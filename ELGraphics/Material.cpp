#include "Material.hpp"
#include "Material_Grid.hpp"
#include "Material_Surface.hpp"
#include "Material_Sprite.hpp"
#include "TextureManager.hpp"
#include <ELCore/MacroUtilities.hpp>

Material* Material::FromText(const String& text, const Context& ctx)
{
	if (text.GetLength())
	{
		size_t splitIndex = text.IndexOfAny("\r\n");
		String firstLine = text.SubString(0, splitIndex).ToLower();
		String relevantData = text.SubString(splitIndex + 1);

		if (firstLine == "surface")
			return Asset::FromText<MaterialSurface>(relevantData, ctx);
		else if (firstLine == "grid")
			return Asset::FromText<MaterialGrid>(relevantData, ctx);
		else if (firstLine == "sprite")
			return Asset::FromText<MaterialSprite>(relevantData, ctx);
	}

	return nullptr;
}
