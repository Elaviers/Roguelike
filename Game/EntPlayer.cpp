#include "EntPlayer.hpp"
#include "Game.hpp"
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
	_camera.SetRelativePosition(Vector3(0.f, 0.f, -5.f));
	_camera.GetProjection().SetNearFar(0.001f, 1000.f);
	_camera.GetProjection().SetType(EProjectionType::PERSPECTIVE);
	//_camera.GetProjection().SetNearFar(-1000.f, 1000.f);
	//_camera.GetProjection().SetOrthographicScale(128.f);
	

	GameInstance::Instance().SetActiveCamera(&_camera);

	ctx.GetPtr<InputManager>()->BindKeyDown(EKeycode::SPACE, Callback(this, &EntPlayer::_Jump));
}

void EntPlayer::_Jump()
{
	_velocity.y += 5.f;
}

void EntPlayer::Update(float deltaTime)
{
	Entity* spinner = GameInstance::Instance().world->RootEntity().FindChild("spin");
	if (spinner)
		spinner->AddRelativeRotation(Vector3(0.f, deltaTime * 30.f, 0.f));

	GameInstance& game = GameInstance::Instance();
	_cameraPivot.AddRelativeRotation(Vector3(-.1f * game.GetAxisLookUp(), .1f * game.GetAxisLookRight(), 0));

	//Overlap printout
	if (false) {
		Buffer<Entity*> ents = GameInstance::Instance().world->RootEntity().FindChildrenOfType<Entity>();
		for (size_t i = 0; i < ents.GetSize(); ++i)
		{
			if (ents[i]->GetUID() != GetUID() && !ents[i]->IsChildOf(this) && ents[i]->Overlaps(*this) == EOverlapResult::OVERLAPPING)
			{
				static int l = 0;
				Debug::PrintLine(CSTR("OVERLAPPING ", l++));
				break;
			}
		}
	}

	//Debug movement (noclip)
	if (false) {
		Transform wt = GetWorldTransform();
		InputManager* inputManager = game.GetGame()->GetContext().GetPtr<InputManager>();
		float z = (inputManager->IsKeyDown(EKeycode::SPACE) ? 1.f : 0.f) - (inputManager->IsKeyDown(EKeycode::LCTRL) ? 1.f : 0.f);
		const float moveFactor = 2.f * deltaTime;
		wt.Move(Vector3(game.GetAxisMoveRight() * moveFactor, z * moveFactor, game.GetAxisMoveForward() * moveFactor));
		SetWorldTransform(wt);
	}

	//Physics movement
	if (true)
	{
		Transform worldTransform = GetWorldTransform();
		Transform cameraT = _camera.GetWorldTransform();

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

		List<Pair<EOverlapResult, Vector3>> overlaps;
		GameInstance::Instance().world->GetOverlaps(overlaps, _COLLIDER, desiredTransform, this);
		//todo: get all world overlaps
		for (const Pair<EOverlapResult, Vector3>& overlap : overlaps) {
			//Pair<Vector3> contacts = ents[i]->GetShallowContactPointsWithCollider(.1f, _COLLIDER, desiredTransform, .1f);
			//Debug::PrintLine(CSTR(contacts.first, "\t", contacts.second));

			const Vector3& penetration = overlap.second;
			if (!(isnan(penetration.x) || isnan(penetration.y) || isnan(penetration.z)))
			{
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
		}

		SetWorldTransform(desiredTransform);
	}
}
