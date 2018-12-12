#include "UIButton.h"

void UIButton::OnMouseMove(float mouseX, float mouseY)
{
	_active = mouseX >= _x && mouseX <= (_x + _w) && mouseY >= _y && mouseY <= (_y + _h);

	_panel.SetColour(_active ? _colourActive : _colourInactive);
}

void UIButton::OnClick()
{
	if (_active)
		_callback(*this);
}
