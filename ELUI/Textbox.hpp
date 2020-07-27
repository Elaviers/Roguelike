#pragma once
#include "Label.hpp"
#include "Panel.hpp"
#include "Rect.hpp"

class UITextbox : public UIElement
{
private:
	float _lastMouseX = 0.f;
	float _lastMouseY = 0.f;

	bool _caretStatus = false;
	float _caretTimer = 0.f;
	size_t _caretPos = 0;

	size_t _selectionStart = 0;
	size_t _selectionEnd = 0;

protected:
	bool _selecting;

	bool _readOnly;

	UIPanel _panel;
	UILabel _label;
	UIRect _selectionRect;

	float _caretWidth;
	float _caretOffset;
	float _caretPeriod;
	UIColour _caretColour;

	virtual void _OnBoundsChanged() override;

	void _OnFocusLost(UIElement& element) { _selectionRect.SetW(0.f); }

	void _ResetCaretBlink();
	void _UpdateSelectionBox();
	void _RemoveSelection();

public:
	Event<UITextbox&> onStringChanged;
	Event<UITextbox&> onStringSubmitted;

	UITextbox(UIElement* parent = nullptr) : UIElement(parent),
		_panel(this), _label(this), _selectionRect(this),
		_selecting(false), _readOnly(false),
		_caretWidth(2.f), _caretOffset(0.f), _caretPeriod(0.33f), _caretColour(Colour::Black)
	{
		SetCursor(ECursor::IBEAM);

		_selectionRect.SetW(0.f);
		SetSelectionColour(Colour(0.f, .2f, 1.f, .2f));

		UIElement::onFocusLost += FunctionPointer<void, UIElement&>(this, &UITextbox::_OnFocusLost);
	}

	virtual ~UITextbox() {}

	UITextbox& SetReadOnly(bool readOnly) { _readOnly = readOnly; return *this; }
	UITextbox& SetCaretColour(const UIColour& colour) { _caretColour = colour; return *this; }
	UITextbox& SetCaretPeriod(float caretPeriod) { _caretPeriod = caretPeriod; return *this; }
	UITextbox& SetCaretOffset(float offset) { _caretOffset = offset; return *this; }
	UITextbox& SetMaterial(const SharedPointer<const Material>& material) { _panel.SetMaterial(material); return *this; }
	UITextbox& SetColour(const UIColour& colour) { _panel.SetColour(colour);  return *this; }
	UITextbox& SetString(const String& string) { _label.SetString(string); onStringChanged(*this); return *this; }
	UITextbox& SetTextColour(const UIColour& colour) { _label.SetColour(colour);  return *this; }
	UITextbox& SetTextShadowColour(const UIColour& colour) { _label.SetShadowColour(colour); return *this; }
	UITextbox& SetTextShadowOffset(const Vector2& offset) { _label.SetShadowOffset(offset); return *this; }
	UITextbox& SetFont(const SharedPointer<const Font>& font) { _label.SetFont(font);  return *this; }
	UITextbox& SetBorderSize(float size) { _panel.SetBorderSize(size);  return *this; }
	UITextbox& SetSelectionColour(const UIColour& colour) { _selectionRect.SetColour(colour); return *this; }

	bool GetReadOnly() const { return _readOnly; }
	const UIColour& GetCaretColour() const { return _caretColour; }
	float GetCaretPeriod() const { return _caretPeriod; }
	float GetCaretOffset() const { return _caretOffset; }
	const SharedPointer<const Material>& GetMaterial() const { return _panel.GetMaterial(); }
	const UIColour& GetColour() const { return _panel.GetColour(); }
	const String& GetString() const { return _label.GetString(); }
	const UIColour& GetTextColour() const { return _label.GetColour(); }
	const UIColour& GetTextShadowColour() const { return _label.GetShadowColour(); }
	const Vector2& GetTextShadowOffset() const { return _label.GetShadowOffset(); }
	const SharedPointer<const Font>& GetFont() const { return _label.GetFont(); }
	float GetBorderSize() const { return _panel.GetBorderSize(); }
	const UIColour& GetSelectionColour() const { return _selectionRect.GetColour(); }

	virtual void Render(RenderQueue&) const override;
	virtual void Update(float deltaTime) override;

	virtual bool OnKeyDown(EKeycode) override;
	virtual bool OnKeyUp(EKeycode) override;
	virtual bool OnCharInput(char) override;
	virtual bool OnMouseUp() override;
	virtual bool OnMouseDown() override;

	virtual void OnMouseMove(float x, float y) override;
};
