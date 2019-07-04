#include "Font.hpp"
#include "Font_Texture.hpp"
#include "Font_TTF.hpp"

Font* Font::FromText(const String& text)
{
	if (text.GetLength())
	{
		int splitIndex = text.IndexOfAny("\r\n");
		String firstLine = text.SubString(0, splitIndex).ToLower();
		String relevantData = text.SubString(splitIndex + 1);

		if (firstLine == "bitmap")
			return Asset::FromText<FontTexture>(relevantData);
		else if (firstLine == "ttf")
			return Asset::FromText<FontTTF>(relevantData);
	}

	return nullptr;
}
