#pragma once
#include "ResourceManager.h"
#include "Font.h"

class FontManager : public ResourceManager<Font>
{

	virtual bool _CreateResource(Font&, const String&, const String&) override;

public:
	FontManager() {}
	virtual ~FontManager() {}
};

