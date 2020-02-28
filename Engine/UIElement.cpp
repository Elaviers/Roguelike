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
		_absoluteBounds.x = _parent->_absoluteBounds.x + (_parent->_absoluteBounds.w * _bounds.x.relative) + _bounds.x.absolute;
		_absoluteBounds.y = _parent->_absoluteBounds.y + (_parent->_absoluteBounds.h * _bounds.y.relative) + _bounds.y.absolute;
		_absoluteBounds.w = _parent->_absoluteBounds.w * _bounds.w.relative + _bounds.w.absolute;
		_absoluteBounds.h = _parent->_absoluteBounds.h * _bounds.h.relative + _bounds.h.absolute;
	}
	else
	{
		_absoluteBounds.x = _bounds.x.relative + _bounds.x.absolute;
		_absoluteBounds.y = _bounds.y.relative + _bounds.y.absolute;
		_absoluteBounds.w = _bounds.w.relative + _bounds.w.absolute;
		_absoluteBounds.h = _bounds.h.relative + _bounds.h.absolute;
	}

	_OnBoundsChanged();
}
