#include "EntPlayer.h"
#include <Engine/InputManager.hpp>
#include <Engine/ModelManager.hpp>

EntPlayer::EntPlayer()
{
	_mesh.SetParent(this);
	_mesh.SetModel(Engine::Instance().pModelManager->Get("player"));
}

void EntPlayer::Update(float deltaTime)
{
	
}
