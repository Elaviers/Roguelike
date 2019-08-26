#include "Material.hpp"
#include "Material_Grid.hpp"
#include "Material_Surface.hpp"
#include "Material_Sprite.hpp"

Material* Material::FromText(const String& text)
{
	if (text.GetLength())
	{
		size_t splitIndex = text.IndexOfAny("\r\n");
		String firstLine = text.SubString(0, splitIndex).ToLower();
		String relevantData = text.SubString(splitIndex + 1);

		if (firstLine == "surface")
			return Asset::FromText<MaterialSurface>(relevantData);
		else if (firstLine == "grid")
			return Asset::FromText<MaterialGrid>(relevantData);
		else if (firstLine == "sprite")
			return Asset::FromText<MaterialSprite>(relevantData);
	}

	return nullptr;
}
