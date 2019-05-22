#include "FontManager.hpp"
#include "Font.hpp"
#include "Font_Texture.hpp"
#include "Font_TTF.hpp"

bool FontManager::_CreateResource(Font*& font, const String& name, const String& data)
{
	if (data.GetLength() != 0)
	{
		int splitIndex = data.IndexOfAny("\r\n");
		String firstLine = data.SubString(0, splitIndex).ToLower();
		Font *newFont;

		if (firstLine == "bitmap")
			newFont = new FontTexture();
		else if (firstLine == "ttf")
			newFont = new FontTTF();
		else
		{
			Debug::Error(CSTR("Unknown type for material \"" + name + '\"'));
			return false;
		}

		if (data[splitIndex + 1] != '\0')
			newFont->FromString(data.SubString(splitIndex + 1));

		font = newFont;
		return true;
	}

	return false;
}
