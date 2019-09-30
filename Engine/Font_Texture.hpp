#pragma once
#include "Font.hpp"
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

	byte _mips;
	uint16 _mag;

	String _GetMag() { return "{MATERIAL._MAG}"; }
	void _SetMag(const String&);

	void _CMD_texture(const Buffer<String>& args);
	void _CMD_region(const Buffer<String>& args);

protected:
	virtual void _ReadText(const String&) override;

public:
	FontTexture() : _texture(nullptr) {}

	virtual ~FontTexture() {}

	virtual const PropertyCollection& GetProperties() override;

	virtual float CalculateStringWidth(const char* string, float scaleX) const override;

	virtual void RenderString(const char* string, const Transform & transform, float lineHeight) const override;

	inline void RenderString(const char* string, const Transform& transform) const { RenderString(string, transform, -transform.GetScale()[1]); }

	inline void BindTexture() const { if (_texture) _texture->Bind(0); }
};

