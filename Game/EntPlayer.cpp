#include "EntPlayer.hpp"
#include "GameInstance.hpp"
#include <ELCore/Context.hpp>
#include <ELPhys/CollisionBox.hpp>
#include <ELPhys/CollisionCapsule.hpp>
#include <ELSys/InputManager.hpp>
#include <Engine/ModelManager.hpp>

Collider EntPlayer::_COLLIDER = Collider(ECollisionChannels::PLAYER, CollisionCapsule(1.f, .5f));

void EntPlayer::Init(const Context& ctx)
{
	_mesh.SetParent(this);
	_mesh.SetModel("capsule", ctx);
	_mesh.SetMaterial("white", ctx);
	_mesh.SetRelativeScale(Vector3(.5f, .5f, .5f));

	_cameraPivot.SetParent(this);
	_cameraPivot.SetRelativeRotation(Vector3(-30, 45, 0));

	_camera.SetParent(&_cameraPivot);
	_camera.GetProjection().SetNearFar(-1000.f, 1000.f);
	_camera.GetProjection().SetType(EProjectionType::ORTHOGRAPHIC);
	_camera.GetProjection().SetOrthographicScale(128.f);

	GameInstance::Instance().SetActiveCamera(&_camera);

	ctx.GetPtr<InputManager>()->BindKeyDown(EKeycode::SPACE, Callback(this, &EntPlayer::_Jump));
}

void EntPlayer::_Jump()
{
	_velocity.y += 5.f;
}

void EntPlayer::Update(float deltaTime)
{
	Entity* spinner = GameInstance::Instance().world->FindChildWithName("spin");
	if (spinner)
		spinner->AddRelativeRotation(Vector3(0.f, deltaTime * 30.f, 0.f));

	GameInstance& game = GameInstance::Instance();

	_cameraPivot.AddRelativeRotation(Vector3(.1f * game.GetAxisLookUp(), .1f * game.GetAxisLookRight(), 0));

	Transform worldTransform = GetWorldTransform();
	Transform cameraT = _camera.GetWorldTransform();
	
	//Debug::PrintLine(String::From(_velocity).GetData());

	_velocity.y -= 9.8f * deltaTime;

	Vector3 dv;
	float amountForward = game.GetAxisMoveForward() * 10;
	if (amountForward)
		dv += cameraT.GetForwardVector() * (deltaTime * amountForward);

	float amountRight = game.GetAxisMoveRight() * 10;
	if (amountRight)
		dv += cameraT.GetRightVector() * (deltaTime * amountRight);

	_velocity.x += dv.x;
	_velocity.z += dv.z;
	

	Vector3 movement = _velocity * deltaTime;

	Transform desiredTransform = Transform(worldTransform.GetPosition() + movement, worldTransform.GetRotation(), worldTransform.GetScale());

	Vector3 penetration;
	Buffer<Entity*> ents = GameInstance::Instance().world->FindChildrenOfType<Entity>();
	for (size_t i = 0; i < ents.GetSize(); ++i)
		if (ents[i]->GetUID() != GetUID() && 
			!ents[i]->IsChildOf(this) && 
			ents[i]->OverlapsCollider(_COLLIDER, desiredTransform, Vector3(), &penetration) == EOverlapResult::OVERLAPPING)
		{
			//Pair<Vector3> contacts = ents[i]->GetShallowContactPointsWithCollider(.1f, _COLLIDER, desiredTransform, .1f);
			//Debug::PrintLine(CSTR(contacts.first, "\t", contacts.second));

			if (isnan(penetration.x) || isnan(penetration.y) || isnan(penetration.z)) continue;

			if (penetration.LengthSquared())
			{
				if (_velocity.LengthSquared())
				{
					Vector3 forbiddenDir = -penetration.Normalised();

					_velocity -= 1.5f * forbiddenDir * forbiddenDir.Dot(_velocity);
				}

				if (Maths::AlmostEquals(_velocity.y, 0.f, 0.1f))
				{
					_velocity.x *= .998f;
					_velocity.z *= .998f;
				}

				desiredTransform.Move(penetration);
			}
		}

	SetWorldTransform(desiredTransform);
}
