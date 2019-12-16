#include "EntPlayer.h"
#include <Engine/InputManager.hpp>
#include <Engine/ModelManager.hpp>
#include "GameInstance.h"

Collider EntPlayer::_COLLIDER = Collider(CollisionChannels::PLAYER, 1.f);

EntPlayer::EntPlayer()
{
	_mesh.SetParent(this);
	_mesh.SetModel("sphere");
	_mesh.SetMaterial("white");
	_mesh.SetRelativeScale(Vector3(.5f, .5f, .5f));

	_cameraPivot.SetParent(this);
	_cameraPivot.SetRelativeRotation(Vector3(-20, 0, 0));

	_camera.SetParent(&_cameraPivot);
	_camera.SetRelativePosition(Vector3(0, 0, -2));
	
	GameInstance::Instance().SetActiveCamera(&_camera);
}

void EntPlayer::Update(float deltaTime)
{
	GameInstance& game = GameInstance::Instance();

	_cameraPivot.AddRelativeRotation(Vector3(100.f * deltaTime * game.GetAxisLookUp(), 100.f * deltaTime * game.GetAxisLookRight(), 0));
	
	Transform desiredTransform = GetWorldTransform();

	Transform cameraT = _camera.GetWorldTransform();
	Vector3 movement = cameraT.GetForwardVector() * deltaTime * game.GetAxisMoveForward() + cameraT.GetRightVector() * deltaTime * game.GetAxisMoveRight();
	movement[1] = 0.f;

	desiredTransform.Move(movement);

	_velocity[1] -= 9.8f * deltaTime;

	Buffer<Entity*> ents = Engine::Instance().pWorld->FindChildrenOfType<Entity>();
	for (size_t i = 0; i < ents.GetSize(); ++i)
	{
		if (ents[i]->OverlapsCollider(_COLLIDER, desiredTransform))
		{
			return;
		}
	}

	SetWorldPosition(desiredTransform.GetPosition());
}
