#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Font.h"

class FontTTF : public Font
{
private:

	struct TTFGlyph
	{
		GLuint texID;

		Vector2 size;
		Vector2 bearing;

		FT_Pos advance;
	};



	FT_Face _face;

	Map<char, TTFGlyph> _charMap;

	GLuint _vao, _vbo;

	int _size;

	void _CMD_LoadFont(const Buffer<String>& args);

public:
	FontTTF() : _face(nullptr), _vao(0), _vbo(0), _size(0) 
	{
		_cvars.CreateVar("filename", CommandPtr(this, &FontTTF::_CMD_LoadFont));
		_cvars.Add("rendersize", _size);
	}

	virtual ~FontTTF() {}

	virtual float CalculateStringWidth(const char* string, float scaleX) const override;

	virtual void RenderString(const char *string, const Transform &transform, float lineHeight) const override;
};
