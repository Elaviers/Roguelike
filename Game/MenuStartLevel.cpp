#include "MenuStartLevel.hpp"
#include "LevelGeneration.hpp"
#include <ELSys/IO.hpp>

MenuStartLevel::~MenuStartLevel()
{
	for (uint32 i = 0; i < _children.GetSize(); ++i)
		delete _children[i];
}

void MenuStartLevel::Initialise(const FunctionPointer<void, const String&> &onLevelChosen)
{
	_onLevelChosen = onLevelChosen;

	_panel.SetParent(this);
	_panel.SetMaterial(_buttonMaterial);
	_panel.SetBorderSize(-8.f);
	_panel.SetColour(UIColour(Colour(.2f, .2f, .2f), Colour::White));

	Buffer<String> filenames = IO::FindFilesInDirectoryRecursive(LevelGeneration::ROOT_DIR, "*.*");

	const float btnH = 32.f;

	for (uint32 i = 0; i < filenames.GetSize(); ++i)
	{
		UITextButton *button = new UITextButton();
		button->SetFont(_buttonFont);
		button->SetBounds(0.f, UICoord(1.f, -((i + 1) * btnH)), 1.f, UICoord(0.f, btnH));
		button->SetBorderSize(_buttonBorderSize);
		button->onPressed += FunctionPointer<void, UIButton&>(this, &MenuStartLevel::ButtonLevel);
		button->SetColour(_buttonColourInactive);
		button->SetColourHold(_buttonColourActive);
		button->SetMaterial(_buttonMaterial);
		button->SetString(filenames[i]);
		button->SetParent(this);
	}
}

void MenuStartLevel::ButtonLevel(UIButton &button)
{
	_onLevelChosen(String(LevelGeneration::ROOT_DIR) + ((UITextButton&)button).GetString());
}
