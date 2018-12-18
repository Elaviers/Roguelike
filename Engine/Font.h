#pragma once
#include "GLTexture.h"
#include "Glyph.h"
#include "Map.h"
#include "String.h"

class Transform;

class Font
{
	const GLTexture *_texture;
	Map<byte, Glyph> _charMap;

	int _yOffset;
	int _divsX, _divsY;

	void _HandleCommand(const String &command);
public:
	Font();
	~Font();

	void FromString(const String &fileString);

	float CalculateStringWidth(const char *string, float scaleX) const;

	void RenderString(const char *string, const Transform &transform) const;

	inline void BindTexture() const { if (_texture) _texture->Bind(0); }
};
