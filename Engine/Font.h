#pragma once
#include "CvarMap.h"
#include "Engine.h"
#include "GLTexture.h"
#include "Glyph.h"
#include "Map.h"
#include "String.h"
#include "TextureManager.h"
#include "Transform.h"

/*
	Font
*/

class Font
{
protected:
	CvarMap _cvars;

	Font() {}
public:
	virtual ~Font() {}

	virtual void FromString(const String& string) 
	{
		Buffer<String> lines = string.Split("\r\n");
		for (size_t i = 0; i < lines.GetSize(); ++i)
			_cvars.HandleCommand(lines[i].ToLower());
	}

	virtual float CalculateStringWidth(const char* string, float scaleX) const = 0;

	virtual void RenderString(const char* string, const Transform& transform, float lineHeight = 0.f) const = 0;
};
