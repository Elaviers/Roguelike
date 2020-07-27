#pragma once
#include "TextButton.hpp"
#include "ListBox.hpp"

class UIComboBox : public UITextButton
{
private:
	float _lastMouseX = 0.f, _lastMouseY = 0.f;

protected:
	bool _selecting;

	bool _readOnly;

	UIListBox _listBox;

	void _OnPressed(UIButton&);
	void _OnSelectionSubmitted(UILabel&);

	void _UpdateListBounds();

	void _StopSelecting();

	virtual void _OnBoundsChanged()
	{
		UITextButton::_OnBoundsChanged();
		_listBox.UpdateAbsoluteBounds();
	}
public:
	Event<UIComboBox&> onStringChanged;

	UIComboBox(UIElement* parent = nullptr) : UITextButton(parent), _listBox(this), _selecting(false), _readOnly(false)
	{
		UITextButton::onPressed += FunctionPointer<void, UIButton&>(this, &UIComboBox::_OnPressed);
		_listBox.onSelectionSubmitted += FunctionPointer<void, UILabel&>(this, &UIComboBox::_OnSelectionSubmitted);
	}

	virtual ~UIComboBox() {}

	void Add(const String& item) { _listBox.Add(item); _UpdateListBounds(); }

	bool GetReadOnly() const { return _readOnly; }
	float GetListBorderSize() const { return _listBox.GetBorderSize(); }
	const SharedPointer<const Material>& GetListMaterial() const { return _listBox.GetMaterial(); }
	const UIColour& GetListColour() const { return _listBox.GetColour(); }
	float GetListItemHeight() const { return _listBox.GetItemHeight(); }
	const SharedPointer<const Font>& GetListFont() const { return _listBox.GetFont(); }
	const UIColour& GetListTextColour() const { return _listBox.GetTextColour(); }
	const UIColour& GetListTextShadowColour() const { return _listBox.GetTextShadowColour(); }
	const Vector2& GetListTextShadowOffset() const { return _listBox.GetTextShadowOffset(); }
	const UIColour& GetListSelectionColour() const { return _listBox.GetSelectionColour(); }

	UIComboBox& SetReadOnly(bool readOnly) { _readOnly = readOnly; return *this; }
	UIComboBox& SetListBorderSize(float borderSize) { _listBox.SetBorderSize(borderSize); return *this; }
	UIComboBox& SetListMaterial(const SharedPointer<const Material>& material) { _listBox.SetMaterial(material); return *this; }
	UIComboBox& SetListColour(const UIColour& colour) { _listBox.SetColour(colour); return *this; }
	UIComboBox& SetListItemHeight(float itemHeight) { _listBox.SetItemHeight(itemHeight); return *this; }
	UIComboBox& SetListFont(const SharedPointer<const Font>& font) { _listBox.SetFont(font); return *this; }
	UIComboBox& SetListTextColour(const UIColour& colour) { _listBox.SetTextColour(colour); return *this; }
	UIComboBox& SetListTextShadowColour(const UIColour& colour) { _listBox.SetTextShadowColour(colour); return *this; }
	UIComboBox& SetListTextShadowOffset(const Vector2& offset) { _listBox.SetTextShadowOffset(offset); return *this; }
	UIComboBox& SetListSelectionColour(const UIColour& colour) { _listBox.SetSelectionColour(colour); return *this; }

	virtual void Render(RenderQueue& q) const override
	{
		UITextButton::Render(q);

		if (_selecting)
			_listBox.Render(q);
	}

	virtual void OnMouseMove(float mouseX, float mouseY) override
	{
		if (_readOnly)
			return;

		if (!_selecting)
			UITextButton::OnMouseMove(mouseX, mouseY);
		
		_listBox.OnMouseMove(mouseX, mouseY); 

		_lastMouseX = mouseX;
		_lastMouseY = mouseY;
	}

	virtual bool OnMouseUp() override
	{
		if (_readOnly)
			return false;

		if (_selecting)
		{
			if (_listBox.OverlapsPoint(_lastMouseX, _lastMouseY))
				return _listBox.OnMouseUp();
			else
				_StopSelecting();
		}
		else
			return UITextButton::OnMouseUp();

		return false;
	}
};
