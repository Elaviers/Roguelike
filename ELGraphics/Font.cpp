#include "Font.hpp"
#include "Font_Texture.hpp"
#include "Font_TTF.hpp"
#include "FontManager.hpp"

Font* Font::FromText(const String& text, const Context& ctx)
{
	if (text.GetLength())
	{
		size_t splitIndex = text.IndexOfAny("\r\n");
		String firstLine = text.SubString(0, splitIndex).ToLower();
		String relevantData = text.SubString(splitIndex + 1);

		if (firstLine == "bitmap")
			return Asset::FromText<FontTexture>(relevantData, ctx);
		else if (firstLine == "ttf")
			return Asset::FromText<FontTTF>(relevantData, ctx);
	}

	return nullptr;
}
