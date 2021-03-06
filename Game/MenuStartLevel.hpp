#pragma once
#include <ELUI/Container.hpp>
#include <ELCore/Buffer.hpp>
#include <ELUI/TextButton.hpp>
#include <ELUI/Panel.hpp>

class Font;
class Material;

class MenuStartLevel : protected UIContainer
{
protected:
	Function<void, const String&> _onLevelChosen;

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

	void Initialise(const Function<void, const String&> &onLevelChosen);

	void SetBounds(const UIBounds& bounds) { UIContainer::SetBounds(bounds); }
	void SetButtonBorderSize(float size) { _buttonBorderSize = size; }
	void SetButtonColourInactive(const UIColour& colour) { _buttonColourInactive = colour; }
	void SetButtonColourActive(const UIColour& colour) { _buttonColourActive = colour; }
	void SetButtonFont(const SharedPointer<const Font>& font) { _buttonFont = font; }
	void SetButtonMaterial(const SharedPointer<const Material>& material) { _buttonMaterial = material; }
	void SetParent(UIElement *parent) { UIContainer::SetParent(parent); }
};
