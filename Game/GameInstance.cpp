#include "GameInstance.h"
#include <Engine/Engine.hpp>
#include <Engine/InputManager.hpp>

void GameInstance::SetupInputs()
{
	InputManager& inputManager = *Engine::Instance().pInputManager;

	inputManager.BindAxis(AxisType::MOUSE_Y, &_axisLookUp);
	inputManager.BindAxis(AxisType::MOUSE_X, &_axisLookRight);
	inputManager.BindKeyAxis(Keycode::W, &_axisMoveForward, 1.f);
	inputManager.BindKeyAxis(Keycode::S, &_axisMoveForward, -1.f);
	inputManager.BindKeyAxis(Keycode::D, &_axisMoveRight, 1.f);
	inputManager.BindKeyAxis(Keycode::A, &_axisMoveRight, -1.f);
}

void GameInstance::OnResize(uint16 w, uint16 h)
{
	_w = w;
	_h = h;

	if (_activeCamera)
		_activeCamera->SetViewport(_w, _h);
}
