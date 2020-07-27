#include "MenuMain.hpp"
#include <Engine/EngineInstance.hpp>
#include <ELGraphics/FontManager.hpp>
#include <ELGraphics/MaterialManager.hpp>
#include "MenuStartLevel.hpp"

void MenuMain::Initialise(const FunctionPointer<void, const String&> &onLevelChosen, const Callback &onQuit, EngineInstance& engineInstance)
{
	const UIColour btnColour(Colour::Blue, Colour::Yellow);
	const UIColour btnColourHover(Colour::Blue, Colour(.9f, .9f, .1f));
	const UIColour btnColourHold(Colour::Red, Colour(.6f, .6f, .3f));

	_onLevelChosen = onLevelChosen;
	_onQuit = onQuit;

	SharedPointer<const Material> material = engineInstance.pMaterialManager->Get("uibutton1", engineInstance.context);
	SharedPointer<const Material> panelMat = engineInstance.pMaterialManager->Get("panel", engineInstance.context);
	SharedPointer<const Font> arial = engineInstance.pFontManager->Get("arial", engineInstance.context);

	_buttonStart.SetFont(arial);
	_buttonQuit.SetFont(arial);
	_buttonSingleLevel.SetFont(arial);

	_panel.SetParent(this);
	_panel.SetMaterial(panelMat);
	_panel.SetBorderSize(-8.f);
	
	_buttonStart.SetParent(this);
	_buttonStart.SetBounds(0.f, UICoord(1.f, -64.f), .4f, UICoord(0.f, 64.f));
	_buttonStart.onPressed += FunctionPointer<void, UIButton&>(this, &MenuMain::ButtonStart);
	
	_buttonQuit.SetParent(this);
	_buttonQuit.SetBounds(.6f, UICoord(1.f, -64.f), .4f, UICoord(0.f, 64.f));
	_buttonQuit.onPressed += FunctionPointer<void, UIButton&>(this, &MenuMain::ButtonQuit);

	_buttonSingleLevel.SetParent(this);
	_buttonSingleLevel.SetBounds(0.f, 0.f, 1.f, UICoord(0.f, 64.f));
	_buttonSingleLevel.onPressed += FunctionPointer<void, UIButton&>(this, &MenuMain::ButtonSingleLevel);

	_buttonStart.SetMaterial(material);
	_buttonQuit.SetMaterial(material);
	_buttonSingleLevel.SetMaterial(material);

	_buttonStart.SetBorderSize(2.f);
	_buttonQuit.SetBorderSize(2.f);
	_buttonSingleLevel.SetBorderSize(2.f);

	_buttonStart.SetColour(btnColour);
	_buttonQuit.SetColour(btnColour);
	_buttonSingleLevel.SetColour(btnColour);

	_buttonStart.SetColourHold(btnColourHold);
	_buttonQuit.SetColourHold(btnColourHold);
	_buttonSingleLevel.SetColourHold(btnColourHold);

	_buttonStart.SetColourHover(btnColourHover);
	_buttonQuit.SetColourHover(btnColourHover);
	_buttonSingleLevel.SetColourHover(btnColourHover);

	_buttonStart.SetString("Start");
	_buttonQuit.SetString("Exit");
	_buttonSingleLevel.SetString("Load Level");
}

void MenuMain::ButtonStart(UIButton&)
{
	MarkForDelete();

	MenuStartLevel *levelSelect = new MenuStartLevel();
	levelSelect->SetBounds(_bounds);
	levelSelect->SetButtonBorderSize(16.f);
	levelSelect->SetButtonColourInactive(_buttonStart.GetColour());
	levelSelect->SetButtonColourActive(_buttonStart.GetColourHold());
	levelSelect->SetButtonFont(_buttonStart.GetFont());
	levelSelect->SetButtonMaterial(_buttonStart.GetMaterial());
	levelSelect->SetParent(_parent);
	levelSelect->Initialise(_onLevelChosen);
}

void MenuMain::ButtonQuit(UIButton&)
{
	_onQuit();
}

#include <ELSys/IO.hpp>
const Buffer<Pair<const wchar_t*>> LEVELFILTER({Pair<const wchar_t*>(L"Level File", L"*.lvl")});

void MenuMain::ButtonSingleLevel(UIButton&)
{
	String filename = IO::OpenFileDialog(L"Data/Levels", LEVELFILTER);

	if (filename.GetLength())
	{
		_onLevelChosen(filename);
	}
}
