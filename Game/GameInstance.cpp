#include "GameInstance.hpp"
#include <ELSys/InputManager.hpp>

void GameInstance::SetupInputs(InputManager& inputManager)
{
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
		_activeCamera->GetProjection().SetDimensions(Vector2T(w, h));
}
