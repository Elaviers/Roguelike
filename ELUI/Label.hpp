#pragma once
#include "Element.hpp"
#include "Colour.hpp"
#include <ELCore/SharedPointer.hpp>
#include <ELMaths/Transform.hpp>
#include <ELMaths/Vector2.hpp>
#include <ELGraphics/Font.hpp>

class UILabel : public UIElement
{
	SharedPointer<const Font> _font;
	String _string;

	UIColour _colour;
	UIColour _shadowColour;

	Transform _transform;
	Transform _shadowTransform;

	Vector2 _shadowOffset;

	void _UpdateShadowTransform();
	virtual void _OnBoundsChanged() override;

public:
	UILabel(UIElement *parent) : UIElement(parent), _font(nullptr), _colour(Colour::White), _shadowOffset(2.f, -2.f), _shadowColour(Colour::Black) {}
	virtual ~UILabel() {}

	const SharedPointer<const Font>& GetFont() const { return _font; }
	const String& GetString() const { return _string; }
	const UIColour& GetColour() const { return _colour; }
	const Transform& GetRenderTransform() const { return _transform; }
	const Vector2& GetShadowOffset() const { return _shadowOffset; }
	const UIColour& GetShadowColour() const { return _shadowColour; }

	UILabel& SetFont(const SharedPointer<const Font>& font) { _font = font; _OnBoundsChanged(); return *this; }
	UILabel& SetString(const String& string) { _string = string; _OnBoundsChanged(); return *this; }
	UILabel& SetColour(const UIColour& colour) { _colour = colour; return *this; }
	UILabel& SetShadowOffset(const Vector2& offset) { _shadowOffset = offset; _UpdateShadowTransform(); return *this; }
	UILabel& SetShadowColour(const UIColour& colour) { _shadowColour = colour; return *this; }

	virtual void Render(RenderQueue&) const override;
};
