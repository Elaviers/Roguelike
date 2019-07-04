#include "FontManager.hpp"
#include "Font.hpp"
#include "Font_Texture.hpp"
#include "Font_TTF.hpp"

Font* FontManager::_CreateResource(const String& name, const String& data)
{
	Font* font = Font::FromText(data);

	if (font == nullptr)
		Debug::Error(CSTR("Unknown type for font \"" + name + '\"'));

	return font;
}
