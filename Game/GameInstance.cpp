#include "GameInstance.h"
#include <Engine/Engine.hpp>
#include <Engine/InputManager.hpp>

void GameInstance::SetupInputs()
{
	InputManager& inputManager = *Engine::Instance().pInputManager;

	inputManager.BindAxis(EAxis::MOUSE_Y, &_axisLookUp);
	inputManager.BindAxis(EAxis::MOUSE_X, &_axisLookRight);
	inputManager.BindKeyAxis(EKeycode::W, &_axisMoveForward, 1.f);
	inputManager.BindKeyAxis(EKeycode::S, &_axisMoveForward, -1.f);
	inputManager.BindKeyAxis(EKeycode::D, &_axisMoveRight, 1.f);
	inputManager.BindKeyAxis(EKeycode::A, &_axisMoveRight, -1.f);
}

void GameInstance::OnResize(uint16 w, uint16 h)
{
	_w = w;
	_h = h;

	if (_activeCamera)
		_activeCamera->SetViewport(w, h);
}
