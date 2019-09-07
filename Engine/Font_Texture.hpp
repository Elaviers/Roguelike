#pragma once
#include "Font.hpp"
#include "CvarMap.hpp"
#include "Engine.hpp"
#include "FunctionPointer.hpp"
#include "Glyph.hpp"
#include "String.hpp"
#include "Texture.hpp"
#include "TextureManager.hpp"


class FontTexture : public Font
{
	const Texture* _texture;
	Hashmap<byte, Glyph> _charMap;

	int _size;
	int _rowH;
	int _yOffset;

	void _CMD_texture(const Buffer<String>& args);
	void _CMD_region(const Buffer<String>& args);

protected:
	virtual void _ReadText(const String&) override;

public:
	FontTexture() : _texture(nullptr) {
		_cvars.CreateVar("mag", CommandPtr(Engine::Instance().pTextureManager, &TextureManager::CMD_mag));
		_cvars.CreateVar("mips", CommandPtr(Engine::Instance().pTextureManager, &TextureManager::CMD_mips));
		_cvars.CreateVar("texture", CommandPtr(this, &FontTexture::_CMD_texture));
		_cvars.CreateVar("region", CommandPtr(this, &FontTexture::_CMD_region));
		_cvars.Add("size", _size);
		_cvars.Add("row_h", _rowH);
		_cvars.Add("y_offset", _yOffset);
	}

	virtual ~FontTexture() {}

	virtual float CalculateStringWidth(const char* string, float scaleX) const override;

	virtual void RenderString(const char* string, const Transform & transform, float lineHeight) const override;

	inline void RenderString(const char* string, const Transform& transform) const { RenderString(string, transform, -transform.GetScale()[1]); }

	inline void BindTexture() const { if (_texture) _texture->Bind(0); }
};

