#pragma once
#include <ELUI/TextButton.hpp>
#include <ELUI/Container.hpp>
#include <ELUI/Rect.hpp>

class EngineInstance;

class MenuMain : public UIContainer
{
	UIPanel _panel;
	UITextButton _buttonStart;
	UITextButton _buttonQuit;

	UITextButton _buttonSingleLevel;

	Function<void, const String&> _onLevelChosen;
	Callback _onQuit;

public:
	MenuMain() {}
	virtual ~MenuMain() {}

	void Initialise(const Function<void, const String&> &onLevelChosen, const Callback &onQuit, EngineInstance& engineInstance);

	void ButtonStart(UIButton&);
	void ButtonQuit(UIButton&);

	void ButtonSingleLevel(UIButton&);
};
