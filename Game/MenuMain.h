#pragma once
#include <Engine/UIButton.h>
#include <Engine/UIContainer.h>
#include <Engine/UIRect.h>

class MenuMain : public UIContainer
{
	UIPanel _panel;
	UIButton _buttonStart;
	UIButton _buttonQuit;

	FunctionPointer<void, const String&> _onLevelChosen;
	Callback _onQuit;

public:
	MenuMain() {}
	virtual ~MenuMain() {}

	void Initialise(const FunctionPointer<void, const String&> &onLevelChosen, const Callback &onQuit);

	void ButtonStart(UIButton&);
	void ButtonQuit(UIButton&);
};
