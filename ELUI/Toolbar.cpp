#include "Toolbar.hpp"

void UIToolbar::_OnItemStateChanged(UICheckbox& pressedItem)
{
	static bool changing = false;
	if (!changing)
	{
		changing = true;
		UIToolbarItem* selectedItem = nullptr;

		for (size_t i = 0; i < _children.GetSize(); ++i)
		{
			UIToolbarItem* item = dynamic_cast<UIToolbarItem*>(_children[i]);
			if (item)
			{
				if (item == &pressedItem)
				{
					item->SetState(true);
					selectedItem = item;
				}
				else
					item->SetState(false);
			}
		}

		if (selectedItem)
			onItemSelected(*selectedItem);

		changing = false;
	}
}

void UIToolbar::AddButton(const String& label, const SharedPointer<const Texture>& texture, uint16 userData)
{
	UIToolbarItem* item = new UIToolbarItem(this);
	item->SetUserData(userData).SetMaterial(_material).SetBorderSize(_borderSize).SetTextureFalse(texture).SetTextureTrue(texture)
		.SetColourFalse(_colourFalse).SetColourTrue(_colourTrue).SetColourHover(_colourHover).SetColourHold(_colourHold);
	item->SetBounds(UICoord(0.f, _nextButtonX), 0.f, UICoord(0.f, _buttonW), 1.f);
	item->onStateChanged += FunctionPointer<void, UICheckbox&>(this, &UIToolbar::_OnItemStateChanged);

	_nextButtonX += _buttonW + _buttonGap;
}

void UIToolbar::SelectByUserData(uint16 userData)
{
	for (size_t i = 0; i < _children.GetSize(); ++i)
	{
		UIToolbarItem* item = dynamic_cast<UIToolbarItem*>(_children[i]);
		if (item && item->GetUserData() == userData)
		{
			_OnItemStateChanged(*item);
			return;
		}
	}
}
