#include "FontManager.hpp"
#include "Font.hpp"
#include "Font_Texture.hpp"
#include "Font_TTF.hpp"

Font* FontManager::_CreateResource(const String& name, const String& data)
{
	Font* font = nullptr;

	if (data.GetLength() > 0)
	{
		int splitIndex = data.IndexOfAny("\r\n");
		String firstLine = data.SubString(0, splitIndex).ToLower();

		if (firstLine == "bitmap")
			font = new FontTexture();
		else if (firstLine == "ttf")
			font = new FontTTF();
		else
		{
			Debug::Error(CSTR("Unknown type for material \"" + name + '\"'));
			return false;
		}

		if (data[splitIndex + 1] != '\0')
			font->FromString(data.SubString(splitIndex + 1));
	}

	return font;
}
