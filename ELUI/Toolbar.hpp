#pragma once
#include "Container.hpp"
#include "Checkbox.hpp"

class UIToolbarItem : public UICheckbox
{
	uint16 _userData;

public:
	UIToolbarItem(UIElement* parent = nullptr) : UICheckbox(parent), _userData(0) {}
	virtual ~UIToolbarItem() {}

	uint16 GetUserData() const { return _userData; }
	UIToolbarItem& SetUserData(uint16 userData) { _userData = userData; return *this; }
};

class UIToolbar : public UIContainer
{
	SharedPointer<const Material> _material;
	float _borderSize;

	UIColour _colourFalse;
	UIColour _colourTrue;
	UIColour _colourHover;
	UIColour _colourHold;

	float _buttonW;
	float _buttonGap;
	float _nextButtonX;

	void _OnItemStateChanged(UICheckbox&);

public:
	Event<UIToolbarItem&> onItemSelected;

	UIToolbar(UIElement* parent = nullptr) : UIContainer(parent), _borderSize(0.f), _buttonW(64.f), _buttonGap(8.f), _nextButtonX(0.f) {}
	virtual ~UIToolbar() {}

	void AddButton(const String& name, const SharedPointer<const Texture>& texture, uint16 userData);

	void SelectByUserData(uint16 userData);

	const SharedPointer<const Material> GetButtonMaterial() const { return _material; }
	float GetButtonBorderSize() const { return _borderSize; }
	float GetButtonWidth() const { return _buttonW; }
	float GetButtonGap() const { return _buttonGap; }
	const UIColour& GetButtonColourFalse() const { return _colourFalse; }
	const UIColour& GetButtonColourTrue() const { return _colourTrue; }
	const UIColour& GetButtonColourHover() const { return _colourHover; }
	const UIColour& GetButtonColourHold() const { return _colourHold; }

	UIToolbar& SetButtonMaterial(const SharedPointer<const Material> &material) { _material = material; return *this; }
	UIToolbar& SetButtonBorderSize(float borderSize) { _borderSize = borderSize; return *this; }
	UIToolbar& SetButtonWidth(float width) { _buttonW = width; return *this; }
	UIToolbar& SetButtonGap(float gap) { _buttonGap = gap; return *this; }
	UIToolbar& SetButtonColourFalse(const UIColour& colour) { _colourFalse = colour; return *this; }
	UIToolbar& SetButtonColourTrue(const UIColour& colour) { _colourTrue = colour; return *this; }
	UIToolbar& SetButtonColourHover(const UIColour& colour) { _colourHover = colour; return *this; }
	UIToolbar& SetButtonColourHold(const UIColour& colour) { _colourHold = colour; return *this; }
};
