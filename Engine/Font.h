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

	Bitmap Font
*/

class Font
{
	CvarMap _cvars;

	const GLTexture *_texture;
	Map<byte, Glyph> _charMap;

	int _size;
	int _rowH;
	int _yOffset;

	void _CMD_texture(const Buffer<String> &args);
	void _CMD_region(const Buffer<String> &args);

public:
	Font() : _texture(nullptr) { 
		_cvars.CreateVar("mag", CommandPtr(Engine::Instance().pTextureManager, &TextureManager::CMD_mag));
		_cvars.CreateVar("mips", CommandPtr(Engine::Instance().pTextureManager, &TextureManager::CMD_mips));
		_cvars.CreateVar("texture", CommandPtr(this, &Font::_CMD_texture));
		_cvars.CreateVar("region", CommandPtr(this, &Font::_CMD_region));
		_cvars.Add("size", _size); 
		_cvars.Add("row_h", _rowH); 
		_cvars.Add("y_offset", _yOffset);
	}

	~Font() {}

	void FromString(const String &fileString);

	float CalculateStringWidth(const char *string, float scaleX) const;

	inline void RenderString(const char* string, const Transform& transform) const { RenderString(string, transform, -transform.GetScale()[1]); }

	void RenderString(const char *string, const Transform& transform, float lineHeight) const;

	inline void BindTexture() const { if (_texture) _texture->Bind(0); }
};
