#include "Material.hpp"
#include "Material_Grid.hpp"
#include "Material_Surface.hpp"
#include "Material_Sprite.hpp"
#include "MacroUtilities.hpp"

const PropertyCollection& Material::GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE_BEGIN;
	properties.Add("mag", MemberGetter<Material, String>(&Material::_GetMag), MemberSetter<Material, String>(&Material::_SetMag));
	properties.Add<byte>("mips", offsetof(Material, _mag));
	DO_ONCE_END;

	return properties;
}

void Material::_SetMag(const String& name)
{

}

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
