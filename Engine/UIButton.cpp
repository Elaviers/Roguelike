#include "UIButton.hpp"

void UIButton::OnMouseMove(float mouseX, float mouseY)
{
	_hover = OverlapsPoint(mouseX, mouseY);

	if (!_hover && _hold)
	{
		_hold = false;
	}

	if (!_hold)
	{
		_panel.SetColour(_hover ? _colourHover : _colour);
	}
}

void UIButton::OnMouseUp()
{
	if (_hold)
	{
		_hold = false;
		onPressed(*this);

		_panel.SetColour(_colourHover);
	}
}

void UIButton::OnMouseDown()
{
	if (_hover)
	{
		_hold = true;
		_panel.SetColour(_colourHold);
	}
}
