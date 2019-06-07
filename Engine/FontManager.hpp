#pragma once
#include "ResourceManager.hpp"

class Font;

class FontManager : public ResourceManager<Font>
{
	virtual Font* _CreateResource(const String&, const String&) override;
	
	String _systemPath;

public:
	FontManager() {}
	virtual ~FontManager() {}

	inline void SetSystemPath(const String& path) { _systemPath = path; }
};

