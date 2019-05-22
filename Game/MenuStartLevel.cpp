#include "MenuStartLevel.hpp"
#include "LevelGeneration.hpp"
#include <Engine/IO.hpp>

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

	Buffer<String> filenames = IO::FindFilesInDirectory(CSTR(LevelGeneration::root + "*.txt"));

	const float btnH = 32.f;

	for (uint32 i = 0; i < filenames.GetSize(); ++i)
	{
		UIButton *button = new UIButton();
		button->SetFont(_buttonFont);
		button->SetBounds(0.f, 1.f, 1.f, btnH, 0.f, -((i + 1) * btnH));
		button->SetBorderSize(_buttonBorderSize);
		button->SetCallback(FunctionPointer<void, UIButton&>(this, &MenuStartLevel::ButtonLevel));
		button->SetColourInactive(_buttonColourInactive);
		button->SetColourActive(_buttonColourActive);
		button->SetMaterial(_buttonMaterial);
		button->SetString(filenames[i]);
		button->SetParent(this);
	}
}

void MenuStartLevel::ButtonLevel(UIButton &button)
{
	_onLevelChosen(String(LevelGeneration::root) + button.GetString());
}
