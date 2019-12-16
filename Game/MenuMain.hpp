#pragma once
#include <Engine/UIButton.hpp>
#include <Engine/UIContainer.hpp>
#include <Engine/UIRect.hpp>

class MenuMain : public UIContainer
{
	UIPanel _panel;
	UIButton _buttonStart;
	UIButton _buttonQuit;

	UIButton _buttonSingleLevel;

	FunctionPointer<void, const String&> _onLevelChosen;
	Callback _onQuit;

public:
	MenuMain() {}
	virtual ~MenuMain() {}

	void Initialise(const FunctionPointer<void, const String&> &onLevelChosen, const Callback &onQuit);

	void ButtonStart(UIButton&);
	void ButtonQuit(UIButton&);

	void ButtonSingleLevel(UIButton&);
};
