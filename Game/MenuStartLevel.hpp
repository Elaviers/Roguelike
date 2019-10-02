#pragma once
#include <Engine/UIContainer.hpp>
#include <Engine/Buffer.hpp>
#include <Engine/UIButton.hpp>
#include <Engine/UIPanel.hpp>
#include <Engine/Vector.hpp>

class Font;
class Material;

class MenuStartLevel : protected UIContainer
{
protected:
	FunctionPointer<void, const String&> _onLevelChosen;

	float _buttonBorderSize;
	Colour _buttonColourInactive;
	Colour _buttonColourActive;
	const Font *_buttonFont;
	const Material *_buttonMaterial;

	UIPanel _panel;

	void ButtonLevel(UIButton&);

public:
	MenuStartLevel(UIElement *parent = nullptr) : UIContainer(parent) {}
	virtual ~MenuStartLevel();

	void Initialise(const FunctionPointer<void, const String&> &onLevelChosen);

	inline void SetBounds(float x, float y, float w, float h, float xOffset = 0.f, float yOffset = 0.f) { UIContainer::SetBounds(x, y, w, h, xOffset, yOffset); }
	inline void SetButtonBorderSize(float size) { _buttonBorderSize = size; }
	inline void SetButtonColourInactive(const Colour& colour) { _buttonColourInactive = colour; }
	inline void SetButtonColourActive(const Colour& colour) { _buttonColourActive = colour; }
	inline void SetButtonFont(const Font *font) { _buttonFont = font; }
	inline void SetButtonMaterial(const Material *material) { _buttonMaterial = material; }
	inline void SetParent(UIElement *parent) { UIContainer::SetParent(parent); }
};
