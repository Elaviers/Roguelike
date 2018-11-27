#pragma once
#include "ResourceManager.h"
#include "Font.h"

class FontManager : public ResourceManager<Font>
{
public:
	FontManager() {}
	virtual ~FontManager() {}

	const Font* GetFont(const String &name);
};

