#include "FontManager.hpp"
#include "Font.hpp"
#include "Font_Texture.hpp"
#include "Font_TTF.hpp"

Font* FontManager::_CreateResource(const String& data, const String& name, const String& ext, const Context& ctx)
{
	Font* font = Font::FromText(data, ctx);

	if (font == nullptr)
		Debug::Error(CSTR("Unknown type for font \"", name, '\"'));

	return font;
}
