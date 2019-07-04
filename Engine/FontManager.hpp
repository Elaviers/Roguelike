#pragma once
#include "AssetManager.hpp"

class Font;

class FontManager : public AssetManager<Font>
{
	virtual Font* _CreateResource(const String&, const String&) override;
	
	String _systemPath;

public:
	FontManager() : AssetManager("") {}
	virtual ~FontManager() {}

	inline void SetSystemPath(const String& path) { _systemPath = path; }
};

