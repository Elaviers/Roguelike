#pragma once
#include "UIElement.hpp"
#include "Colour.hpp"
#include "Font.hpp"
#include "Transform.hpp"

class UILabel : public UIElement
{
	const Font *_font;
	String _string;
	Colour _colour;

	Transform _transform;

public:
	UILabel(UIElement *parent) : UIElement(parent), _font(nullptr), _colour(1.f, 1.f, 1.f, 1.f) {}
	virtual ~UILabel() {}

	inline const Font* GetFont() const { return _font; }
	inline const String& GetString() const { return _string; }
	inline const Colour& GetColour() const { return _colour; }

	inline void SetFont(const Font *font) { _font = font; }
	inline void SetString(const String &string) { _string = string; }
	inline void SetColour(const Colour &colour) { _colour = colour; }

	virtual void Render() const override;

	virtual void OnBoundsChanged() override;
};
