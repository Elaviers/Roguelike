#pragma once
#include <ft2build.h>
#include FT_FREETYPE_H
#include "Font.hpp"
#include <ELCore/Hashmap.hpp>
#include <ELMaths/Vector2.hpp>
#include <ELSys/GL.hpp>

class FontTTF : public Font
{
private:

	struct TTFGlyph
	{
		GLuint texID = 0;

		Vector2 size;
		Vector2 bearing;

		FT_Pos advance = 0;
	};

	FT_Face _face;

	int _descender;

	Hashmap<char, TTFGlyph> _charMap;

	GLuint _vao, _vbo;

	int _size;

	void _CMD_LoadFont(const Buffer<String>& args, const Context& ctx);

public:
	FontTTF() : _face(nullptr), _vao(0), _vbo(0), _size(0), _descender(0) {}

	virtual ~FontTTF() {}

	virtual const PropertyCollection& GetProperties() override;

	virtual float CalculateStringWidth(const char* string, float scaleX, size_t maxChars = 0) const override;
	virtual size_t GetPositionOf(float x, float y, const char* string, const Transform& transform, float lineHeight = 0.f) const override;

	virtual void RenderString(RenderEntry&, const char *string, const Transform &transform, float lineHeight) const override;
};
