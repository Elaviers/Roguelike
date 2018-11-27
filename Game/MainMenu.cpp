#include "MainMenu.h"
#include <Engine/Engine.h>

const Vector4 inactiveBtnColour(.8f, .8f, .2f, 1.f);
const Vector4 activeBtnColour(.6f, .6f, .3f, 1.f);

void MainMenu::Initialise()
{
	_rect.SetParent(this);
	_rect.SetBounds(0, 0, 1, 1);
	
	_buttonStart.SetParent(this);
	_buttonStart.SetBounds(0.f, 1.f, .5f, 32.f, 0.f, -32.f);
	
	_buttonQuit.SetParent(this);
	_buttonQuit.SetBounds(.5f, 1.f, .5f, 32.f, 0.f, -32.f);

	const Material *material = Engine::materialManager->GetMaterial("panel");
	_buttonStart.SetMaterial(material);
	_buttonQuit.SetMaterial(material);

	_buttonStart.SetBorderSize(16.f);
	_buttonQuit.SetBorderSize(16.f);

	_buttonStart.SetColourInctive(inactiveBtnColour);
	_buttonQuit.SetColourInctive(inactiveBtnColour);

	_buttonStart.SetColourActive(activeBtnColour);
	_buttonQuit.SetColourActive(activeBtnColour);

	_buttonStart.SetString("Start");
	_buttonQuit.SetString("Exit");

	const Font *arial = Engine::fontManager->GetFont("arial32");
	_buttonStart.SetFont(arial);
	_buttonQuit.SetFont(arial);
}
