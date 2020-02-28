#include "UIComboBox.hpp"

void UIComboBox::_StopSelecting()
{
	_selecting = false;

	UITextButton::OnMouseMove(_lastMouseX, _lastMouseY);
}

void UIComboBox::_OnPressed(UIButton& button)
{
	_RequestFocus();

	_hover = false;
	_panel.SetColour(_colourHold);
	_selecting = !_selecting;
}

void UIComboBox::_OnSelectionSubmitted(UILabel& label)
{
	SetString(label.GetString());
	_StopSelecting();

	onStringChanged(*this);
}

void UIComboBox::_UpdateListBounds()
{
	float h = _listBox.GetItemHeight() * _listBox.GetItems().GetSize();
	_listBox.SetBounds(0.f, 1.f, 1.f, h, 0.f, -h - _absoluteBounds.h);
}
