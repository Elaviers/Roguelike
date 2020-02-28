#pragma once
#include <Engine/UITextButton.hpp>
#include <Engine/UIContainer.hpp>
#include <Engine/UIRect.hpp>

class MenuMain : public UIContainer
{
	UIPanel _panel;
	UITextButton _buttonStart;
	UITextButton _buttonQuit;

	UITextButton _buttonSingleLevel;

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
