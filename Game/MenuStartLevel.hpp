#pragma once
#include <Engine/UIContainer.hpp>
#include <Engine/Buffer.hpp>
#include <Engine/UITextButton.hpp>
#include <Engine/UIPanel.hpp>
#include <Engine/Vector.hpp>

class Font;
class Material;

class MenuStartLevel : protected UIContainer
{
protected:
	FunctionPointer<void, const String&> _onLevelChosen;

	float _buttonBorderSize = 1.f;
	UIColour _buttonColourInactive;
	UIColour _buttonColourActive;
	SharedPointer<const Font> _buttonFont;
	SharedPointer<const Material> _buttonMaterial;

	UIPanel _panel;

	void ButtonLevel(UIButton&);

public:
	MenuStartLevel(UIElement *parent = nullptr) : UIContainer(parent) {}
	virtual ~MenuStartLevel();

	void Initialise(const FunctionPointer<void, const String&> &onLevelChosen);

	void SetBounds(float x, float y, float w, float h, float xOffset = 0.f, float yOffset = 0.f) { UIContainer::SetBounds(x, y, w, h, xOffset, yOffset); }
	void SetButtonBorderSize(float size) { _buttonBorderSize = size; }
	void SetButtonColourInactive(const UIColour& colour) { _buttonColourInactive = colour; }
	void SetButtonColourActive(const UIColour& colour) { _buttonColourActive = colour; }
	void SetButtonFont(const SharedPointer<const Font>& font) { _buttonFont = font; }
	void SetButtonMaterial(const SharedPointer<const Material>& material) { _buttonMaterial = material; }
	void SetParent(UIElement *parent) { UIContainer::SetParent(parent); }
};
