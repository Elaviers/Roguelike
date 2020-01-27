#include "EntPlayer.h"
#include <Engine/CollisionBox.hpp>
#include <Engine/CollisionCapsule.hpp>
#include <Engine/InputManager.hpp>
#include <Engine/ModelManager.hpp>
#include "GameInstance.h"

Collider EntPlayer::_COLLIDER = Collider(CollisionChannels::PLAYER, CollisionCapsule(1.f, .5f));

EntPlayer::EntPlayer()
{
	_mesh.SetParent(this);
	_mesh.SetModel("capsule");
	_mesh.SetMaterial("white");
	_mesh.SetRelativeScale(Vector3(.5f, .5f, .5f));

	_cameraPivot.SetParent(this);
	_cameraPivot.SetRelativeRotation(Vector3(-20, 0, 0));

	_camera.SetParent(&_cameraPivot);
	_camera.SetRelativePosition(Vector3(0, 0, -2));
	
	GameInstance::Instance().SetActiveCamera(&_camera);

	Engine::Instance().pInputManager->BindKey(Keycode::SPACE, Callback(this, &EntPlayer::_Jump));
}

void EntPlayer::_Jump()
{
	_velocity[1] += 5.f;
}

bool EntPlayer::_TryMovement(const Transform& wt, const Vector3& movement, float testYOffset)
{
	Vector3 invMovement(-1.f * movement);

	Transform t = wt;
	//t.Move(Vector3(0, testYOffset, 0));

	Buffer<Entity*> ents = Engine::Instance().pWorld->FindChildrenOfType<Entity>();
	for (size_t i = 0; i < ents.GetSize(); ++i)
		if (ents[i]->GetUID() != GetUID() && !ents[i]->IsChildOf(this) && ents[i]->OverlapsCollider(_COLLIDER, t, invMovement))
		{
			if (testYOffset)
				Debug::PrintLine(CSTR(ents[i]->GetUID()));

			return false;
		}

	SetWorldPosition(wt.GetPosition() + movement);
	return true;
}

void EntPlayer::Update(float deltaTime)
{
	GameInstance& game = GameInstance::Instance();

	_cameraPivot.AddRelativeRotation(Vector3(100.f * deltaTime * game.GetAxisLookUp(), 100.f * deltaTime * game.GetAxisLookRight(), 0));

	Transform worldTransform = GetWorldTransform();
	Transform cameraT = _camera.GetWorldTransform();
	
	Debug::PrintLine(String::From(worldTransform.GetPosition()[1], 0, 10).GetData());

	_velocity[1] -= 9.8f * deltaTime;

	Vector3 gMovement = Vector3(0.f, _velocity[1] * deltaTime, 0.f);

	bool inAir = _TryMovement(worldTransform, gMovement, 0.f);

	if (inAir)
		worldTransform.Move(gMovement);
	else
		_velocity[1] *= -0.5f;

	Vector3 dv;
	float amountForward = game.GetAxisMoveForward() * 10;
	if (amountForward)
		dv += cameraT.GetForwardVector() * (deltaTime * amountForward);

	float amountRight = game.GetAxisMoveRight() * 10;
	if (amountRight)
		dv += cameraT.GetRightVector() * (deltaTime * amountRight);

	_velocity[0] += dv[0];
	_velocity[2] += dv[2];

	if (_TryMovement(worldTransform, Vector3(_velocity[0] * deltaTime, 0.f, _velocity[2] * deltaTime), 0.f))
	{
		if (!inAir)
		{
			_velocity[0] *= .998f;
			_velocity[2] *= .998f;
		}
	}
	else
	{
		_velocity[0] *= -.5f;
		_velocity[2] *= -.5f;
	}
}
