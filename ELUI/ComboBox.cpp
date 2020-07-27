#include "ComboBox.hpp"

void UIComboBox::_StopSelecting()
{
	_selecting = false;

	UITextButton::OnMouseMove(_lastMouseX, _lastMouseY);
}

void UIComboBox::_OnPressed(UIButton& button)
{
	RequestFocus();

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
	_listBox.SetBounds(0.f, UICoord(1.f, -h - _absoluteBounds.h), 1.f, UICoord(0.f, h));
}
