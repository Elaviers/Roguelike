#include "EntPlayer.h"
#include <Engine/CollisionBox.hpp>
#include <Engine/CollisionCapsule.hpp>
#include <Engine/InputManager.hpp>
#include <Engine/ModelManager.hpp>
#include "GameInstance.h"

Collider EntPlayer::_COLLIDER = Collider(ECollisionChannels::PLAYER, CollisionCapsule(1.f, .5f));

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

	Engine::Instance().pInputManager->BindKey(EKeycode::SPACE, Callback(this, &EntPlayer::_Jump));
}

void EntPlayer::_Jump()
{
	_velocity[1] += 5.f;
}

void EntPlayer::Update(float deltaTime)
{
	Entity* spinner = Engine::Instance().pWorld->FindChildWithName("spin");
	if (spinner)
		spinner->AddRelativeRotation(Vector3(0.f, deltaTime * 30.f, 0.f));

	GameInstance& game = GameInstance::Instance();

	_cameraPivot.AddRelativeRotation(Vector3(.1f * game.GetAxisLookUp(), .1f * game.GetAxisLookRight(), 0));

	Transform worldTransform = GetWorldTransform();
	Transform cameraT = _camera.GetWorldTransform();
	
	//Debug::PrintLine(String::From(_velocity).GetData());

	_velocity[1] -= 9.8f * deltaTime;

	Vector3 dv;
	float amountForward = game.GetAxisMoveForward() * 10;
	if (amountForward)
		dv += cameraT.GetForwardVector() * (deltaTime * amountForward);

	float amountRight = game.GetAxisMoveRight() * 10;
	if (amountRight)
		dv += cameraT.GetRightVector() * (deltaTime * amountRight);

	_velocity[0] += dv[0];
	_velocity[2] += dv[2];
	

	Vector3 movement = _velocity * deltaTime;

	Transform desiredTransform = Transform(worldTransform.GetPosition() + movement, worldTransform.GetRotation(), worldTransform.GetScale());

	Vector3 penetration;
	Buffer<Entity*> ents = Engine::Instance().pWorld->FindChildrenOfType<Entity>();
	for (size_t i = 0; i < ents.GetSize(); ++i)
		if (ents[i]->GetUID() != GetUID() && 
			!ents[i]->IsChildOf(this) && 
			ents[i]->OverlapsCollider(_COLLIDER, desiredTransform, Vector3(), &penetration) == EOverlapResult::OVERLAPPING)
		{
			Pair<Vector3> contacts = ents[i]->GetShallowContactPointsWithCollider(.1f, _COLLIDER, desiredTransform, .1f);
			Debug::PrintLine(CSTR(contacts.first, "\t", contacts.second));

			if (isnan(penetration[0]) || isnan(penetration[1]) || isnan(penetration[2])) continue;

			if (penetration.LengthSquared())
			{
				if (_velocity.LengthSquared())
				{
					Vector3 forbiddenDir = -penetration.Normalised();

					_velocity -= 1.5f * forbiddenDir * Vector3::Dot(forbiddenDir, _velocity);
				}

				if (Maths::AlmostEqual(_velocity[1], 0.f, 0.1f))
				{
					_velocity[0] *= .998f;
					_velocity[2] *= .998f;
				}

				desiredTransform.Move(penetration);
			}
		}

	SetWorldTransform(desiredTransform);
}
