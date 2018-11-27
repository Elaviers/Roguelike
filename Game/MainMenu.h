#pragma once
#include <Engine/UIButton.h>
#include <Engine/UIContainer.h>
#include <Engine/UIRect.h>

class MainMenu : public UIContainer
{
	UIRect _rect;
	UIButton _buttonStart;
	UIButton _buttonQuit;

public:
	MainMenu() {}
	virtual ~MainMenu() {}

	void Initialise();

	inline void SetOnStart(const Callback &callback) { _buttonStart.SetCallback(callback); }
	inline void SetOnQuit(const Callback &callback) { _buttonQuit.SetCallback(callback); }
};

