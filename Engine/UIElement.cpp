#include "UIElement.hpp"

void UIElement::UpdateAbsoluteBounds()
{
	if (_parent)
	{
		if (_bounds.x > 0.f && _bounds.x <= 1.f)	_x = _parent->_x + (_parent->_w * _bounds.x) + _bounds.xOffset;
		else _x = _parent->_x + _bounds.x + _bounds.xOffset;

		if (_bounds.y > 0.f && _bounds.y <= 1.f)	_y = _parent->_y + (_parent->_h * _bounds.y) + _bounds.yOffset;
		else _y = _parent->_y + _bounds.y + _bounds.yOffset;

		if (_bounds.w > 0.f && _bounds.w <= 1.f)	_w = _parent->_w * _bounds.w;
		else _w = _bounds.w;

		if (_bounds.h > 0.f && _bounds.h <= 1.f)	_h = _parent->_h * _bounds.h;
		else _h = _bounds.h;
	}
	else
	{
		_x = _bounds.x + _bounds.xOffset;
		_y = _bounds.y + _bounds.yOffset;
		_w = _bounds.w;
		_h = _bounds.h;
	}

	OnBoundsChanged();
}
