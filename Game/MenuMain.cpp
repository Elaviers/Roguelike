#include "MenuMain.h"
#include <Engine/Engine.h>
#include "MenuStartLevel.h"

const Vector4 inactiveBtnColour(.8f, .8f, .2f, 1.f);
const Vector4 activeBtnColour(.6f, .6f, .3f, 1.f);

void MenuMain::Initialise(const FunctionPointer<void, const String&> &onLevelChosen, const Callback &onQuit)
{
	_onLevelChosen = onLevelChosen;
	_onQuit = onQuit;

	const Material *material = Engine::materialManager->GetMaterial("panel");
	const Font *arial = Engine::fontManager->GetFont("arial32");

	_panel.SetParent(this);
	_panel.SetMaterial(material);
	_panel.SetBorderSize(-8.f);
	
	_buttonStart.SetParent(this);
	_buttonStart.SetBounds(0.f, 1.f, .5f, 32.f, 0.f, -32.f);
	_buttonStart.SetCallback(FunctionPointer<void, UIButton&>(this, &MenuMain::ButtonStart));
	
	_buttonQuit.SetParent(this);
	_buttonQuit.SetBounds(.5f, 1.f, .5f, 32.f, 0.f, -32.f);
	_buttonQuit.SetCallback(FunctionPointer<void, UIButton&>(this, &MenuMain::ButtonQuit));

	_buttonStart.SetMaterial(material);
	_buttonQuit.SetMaterial(material);

	_buttonStart.SetBorderSize(8.f);
	_buttonQuit.SetBorderSize(8.f);

	_buttonStart.SetColourInactive(inactiveBtnColour);
	_buttonQuit.SetColourInactive(inactiveBtnColour);

	_buttonStart.SetColourActive(activeBtnColour);
	_buttonQuit.SetColourActive(activeBtnColour);

	_buttonStart.SetString("Start");
	_buttonQuit.SetString("Exit");

	_buttonStart.SetFont(arial);
	_buttonQuit.SetFont(arial);
}

void MenuMain::ButtonStart(UIButton&)
{
	MarkForDelete();

	MenuStartLevel *levelSelect = new MenuStartLevel();
	levelSelect->SetBounds(_bounds.x, _bounds.y, _bounds.w, _bounds.h, _bounds.xOffset, _bounds.yOffset);
	levelSelect->SetButtonBorderSize(16.f);
	levelSelect->SetButtonColourInactive(_buttonStart.GetColourInactive());
	levelSelect->SetButtonColourActive(_buttonStart.GetColourActive());
	levelSelect->SetButtonFont(_buttonStart.GetFont());
	levelSelect->SetButtonMaterial(_buttonStart.GetMaterial());
	levelSelect->SetParent(_parent);
	levelSelect->Initialise(_onLevelChosen);
}

void MenuMain::ButtonQuit(UIButton&)
{
	_onQuit();
}
