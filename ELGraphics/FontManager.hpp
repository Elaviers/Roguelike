#pragma once
#include <ELSys/AssetManager.hpp>
#include "Font.hpp"

class FontManager : public AssetManager<Font>
{
	virtual Font* _CreateResource(const String&, const String&, const String&, const Context&) override;
	
	String _systemPath;

public:
	FontManager() : AssetManager() {}
	virtual ~FontManager() {}

	void SetSystemPath(const String& path) { _systemPath = path; }
};
