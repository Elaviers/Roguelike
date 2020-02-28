#pragma once
#include "Asset.hpp"
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
	Font() {}

public:
	virtual ~Font() {}

	static Font* FromText(const String& string);

	virtual float CalculateStringWidth(const char* string, float scaleX, size_t maxChars = 0) const = 0;
	virtual size_t GetPositionOf(float x, float y, const char* string, const Transform& transform, float lineHeight = 0.f) const = 0;

	virtual void RenderString(const char* string, const Transform& transform, float lineHeight = 0.f) const = 0;
};
