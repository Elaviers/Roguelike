#pragma once
#include "Font.hpp"
#include "Engine.hpp"
#include "FunctionPointer.hpp"
#include "Glyph.hpp"
#include "String.hpp"
#include "Texture.hpp"

class FontTexture : public Font
{
	SharedPointer<const Texture> _texture;
	Hashmap<byte, Glyph> _charMap;

	int _size = 0;
	int _rowH = 0;
	int _yOffset = 0;

	void _CMD_texture(const Buffer<String>& args);
	void _CMD_region(const Buffer<String>& args);

protected:
	virtual void _ReadText(const String&) override;

public:
	FontTexture() : _texture(nullptr) {}

	virtual ~FontTexture() {}

	virtual const PropertyCollection& GetProperties() override;

	virtual float CalculateStringWidth(const char* string, float scaleX, size_t maxChars = 0) const override;
	virtual size_t GetPositionOf(float x, float y, const char* string, const Transform& transform, float lineHeight = 0.f) const override;

	virtual void RenderString(const char* string, const Transform & transform, float lineHeight) const override;

	void RenderString(const char* string, const Transform& transform) const { RenderString(string, transform, -transform.GetScale().y); }

	void BindTexture() const { if (_texture) _texture->Bind(0); }
};

