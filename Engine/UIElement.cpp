#include "UIElement.hpp"

void UIElement::_RequestFocus()
{
	if (_parent)
	{
		UIElement* root = _parent;
		while (root->_parent)
			root = root->_parent;

		root->OnElementRequestFocus(this);
	}
}

void UIElement::OnElementRequestFocus(UIElement* element)
{
	if (element == this)
		onFocusGained(*this);
	else
		onFocusLost(*this);
}

void UIElement::UpdateAbsoluteBounds()
{
	if (_parent)
	{
		if (_relativeBounds.x > 0.f && _relativeBounds.x <= 1.f)	
			_absoluteBounds.x = _parent->_absoluteBounds.x + (_parent->_absoluteBounds.w * _relativeBounds.x) + _relativeBounds.xOffset;
		else 
			_absoluteBounds.x = _parent->_absoluteBounds.x + _relativeBounds.x + _relativeBounds.xOffset;

		if (_relativeBounds.y > 0.f && _relativeBounds.y <= 1.f)
			_absoluteBounds.y = _parent->_absoluteBounds.y + (_parent->_absoluteBounds.h * _relativeBounds.y) + _relativeBounds.yOffset;
		else
			_absoluteBounds.y = _parent->_absoluteBounds.y + _relativeBounds.y + _relativeBounds.yOffset;

		if (_relativeBounds.w > 0.f && _relativeBounds.w <= 1.f)
			_absoluteBounds.w = _parent->_absoluteBounds.w * _relativeBounds.w + _relativeBounds.wOffset;
		else 
			_absoluteBounds.w = _relativeBounds.w + _relativeBounds.wOffset;

		if (_relativeBounds.h > 0.f && _relativeBounds.h <= 1.f)
			_absoluteBounds.h = _parent->_absoluteBounds.h * _relativeBounds.h + _relativeBounds.hOffset;
		else 
			_absoluteBounds.h = _relativeBounds.h + _relativeBounds.hOffset;
	}
	else
	{
		_absoluteBounds.x = _relativeBounds.x + _relativeBounds.xOffset;
		_absoluteBounds.y = _relativeBounds.y + _relativeBounds.yOffset;
		_absoluteBounds.w = _relativeBounds.w + _relativeBounds.wOffset;
		_absoluteBounds.h = _relativeBounds.h + _relativeBounds.hOffset;
	}

	_OnBoundsChanged();
}
