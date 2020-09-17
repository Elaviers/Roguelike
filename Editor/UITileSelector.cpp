#include "UITileSelector.hpp"
#include <ELGraphics/Material_Sprite.hpp>

void UITileSelector::_OnBoundsChanged()
{
	UIContainer::_OnBoundsChanged();
	
	float bs = _panel.GetBorderSize();
	float size = Maths::Min(_absoluteBounds.w - bs * 2.f, _absoluteBounds.h - bs * 2.f);
	_rect.SetBounds(UICoord(0.f, bs), UICoord(0.f, bs), UICoord(0.f, size), UICoord(0.f, size));
}

UITileSelector::UITileSelector() : _hold(false), _selected(false)
{
	_panel.SetParent(this);
	_rect.SetParent(this);
}

void UITileSelector::SetTile(const SharedPointer<const Tile>& tile)
{
	_tile = tile;

	const MaterialSprite* sprite = dynamic_cast<const MaterialSprite*>(_tile->GetMaterial().Ptr());
	if (sprite)
		_rect.SetTexture(sprite->GetDiffuse());
}

void UITileSelector::OnHoverStart()
{
	if (!_selected)
		_panel.SetColour(_panelColourHover);
}

void UITileSelector::OnHoverStop()
{
	if (!_selected)
		_panel.SetColour(_panelColour);
}

bool UITileSelector::OnKeyDown(bool blocked, EKeycode key)
{
	if (key == EKeycode::MOUSE_LEFT && !blocked && _hover)
	{
		_hold = true;
		return true;
	}

	return false;
}

bool UITileSelector::OnKeyUp(bool blocked, EKeycode key)
{
	if (key == EKeycode::MOUSE_LEFT && !blocked && _hold)
	{
		onPressed(*this);
		_hold = false;
		return true;
	}

	_hold = false;
	return false;
}

bool UITileSelector::OnMouseMove(bool blocked, float x, float y)
{
	return UIElement::OnMouseMove(blocked, x, y);
}
