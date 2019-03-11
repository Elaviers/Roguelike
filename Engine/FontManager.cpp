#include "FontManager.h"

bool FontManager::_CreateResource(Font& font, const String& name, const String& data)
{
	font.FromString(data);
	return true;
}
