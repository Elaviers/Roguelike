#pragma once
#include "UIElement.h"
#include "Font.h"
#include "Transform.h"

class UILabel : public UIElement
{
	const Font *_font;
	String _string;
	Vector4 _colour;

	Transform _transform;

public:
	UILabel(UIElement *parent) : UIElement(parent), _colour(1.f, 1.f, 1.f, 1.f) {}
	virtual ~UILabel() {}

	inline void SetFont(const Font *font) { _font = font; }
	inline void SetString(const String &string) { _string = string; }
	inline void SetColour(const Vector4 &colour) { _colour = colour; }

	virtual void Render() const override;

	virtual void OnBoundsChanged() override;
};
