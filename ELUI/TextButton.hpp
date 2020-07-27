#pragma once
#include "Button.hpp"

class UITextButton : public UIButton
{
protected:
	UILabel _label;

	virtual void _OnBoundsChanged() override
	{
		UIButton::_OnBoundsChanged();
		_label.UpdateAbsoluteBounds();
	}

public:
	UITextButton(UIElement* parent = nullptr) : UIButton(parent), _label(this) { }
	virtual ~UITextButton() {}

	const UIColour& GetTextColour() const { return _label.GetColour(); }
	const UIColour& GetTextShadowColour() const { return _label.GetShadowColour(); }
	const Vector2& GetTextShadowOffset() const { return _label.GetShadowOffset(); }
	const SharedPointer<const Font>& GetFont() const { return _label.GetFont(); }
	const String& GetString() const { return _label.GetString(); }

	UITextButton& SetTextColour(const UIColour& colour) { _label.SetColour(colour); return *this; }
	UITextButton& SetTextShadowColour(const UIColour& colour) { _label.SetShadowColour(colour); return *this; }
	UITextButton& SetTextShadowOffset(const Vector2& offset) { _label.SetShadowOffset(offset); return *this; }
	UITextButton& SetFont(const SharedPointer<const Font>& font) { _label.SetFont(font); return *this; }
	UITextButton& SetString(const String& string) { _label.SetString(string); return *this; }

	virtual void Render(RenderQueue& q) const override
	{
		UIButton::Render(q);
		_label.Render(q);
	}
};
