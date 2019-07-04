#pragma once
#include "Asset.hpp"
#include "CvarMap.hpp"
#include "Engine.hpp"
#include "GLTexture.hpp"
#include "Glyph.hpp"
#include "Map.hpp"
#include "String.hpp"
#include "TextureManager.hpp"
#include "Transform.hpp"

/*
	Font
*/

class Font : public Asset
{
protected:
	CvarMap _cvars;

	Font() {}

	virtual void _ReadText(const String& string) override
	{
		Buffer<String> lines = string.Split("\r\n");
		for (size_t i = 0; i < lines.GetSize(); ++i)
			_cvars.HandleCommand(lines[i].ToLower());
	}
public:
	virtual ~Font() {}

	static Font* FromText(const String& string);

	virtual float CalculateStringWidth(const char* string, float scaleX) const = 0;

	virtual void RenderString(const char* string, const Transform& transform, float lineHeight = 0.f) const = 0;
};
