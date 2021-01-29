#include "EntPlayer.hpp"
#include "Game.hpp"
#include "GameInstance.hpp"
#include <ELCore/Context.hpp>
#include <ELPhys/Collision.hpp>
#include <ELPhys/CollisionBox.hpp>
#include <ELPhys/CollisionCapsule.hpp>
#include <ELSys/InputManager.hpp>
#include <Engine/ModelManager.hpp>

Collider EntPlayer::_COLLIDER = Collider(ECollisionChannels::PLAYER, CollisionCapsule(1.f, .5f));

void EntPlayer::_OnTransformChanged()
{
	_rootCameraTransform = GetWorldTransform();
	_rootCameraTransform.SetRotation(_camera.GetWorldRotation());
	_rootCameraTransform.Move(_camera.GetWorldTransform().GetForwardVector() * -5000.f);
}

EntPlayer::EntPlayer()
{
	onTransformChanged += FunctionPointer(this, &EntPlayer::_OnTransformChanged);
}

void EntPlayer::Init(const Context& ctx)
{
	_mesh.SetParent(this);
	_mesh.SetModel("capsule", ctx);
	_mesh.SetMaterial("white", ctx);
	_mesh.SetRelativeScale(Vector3(.5f, .5f, .5f));

	_camera.SetParent(this);
	_camera.GetProjection().SetType(EProjectionType::ORTHOGRAPHIC);
	_camera.SetRelativeRotation(Vector3(Maths::RadiansToDegrees(-Maths::ArcTan(1.f / Maths::SQRT2_F)), 45.f, 0.f));
	_camera.GetProjection().SetNearFar(0.f, 10000.f);
	_camera.GetProjection().SetOrthographicScale(64.f);

	GameInstance::Instance().SetActiveCamera(&_camera);

	ctx.GetPtr<InputManager>()->BindKeyDown(EKeycode::SPACE, Callback(this, &EntPlayer::_Jump));

	_targetTexture = ctx.GetPtr<TextureManager>()->Get("iso/targ_temp", ctx);
}

void EntPlayer::_Jump()
{
	_velocity.y += 5.f;
}

float CalculateAccel(float inputAxis, float currentVel, float deltaTime, float scaleFactor = 1.f)
{
	const float moveVel = 10.f * scaleFactor;
	const float accel = 20.f * scaleFactor;
	const float deaccel = 10.f * scaleFactor;

	float desiredVel = inputAxis * moveVel * scaleFactor;

	if (currentVel != desiredVel)
	{
		float accelAmount = Maths::Lerp(deaccel, accel, Maths::Abs(desiredVel) / moveVel);
		float relAccel = currentVel < desiredVel ? accelAmount : -accelAmount;
		currentVel += relAccel * deltaTime;

		if (relAccel > 0.f)
		{
			if (currentVel > desiredVel)
				currentVel = desiredVel;
		}
		else if (currentVel < desiredVel)
			currentVel = desiredVel;
	}

	return currentVel;
}

void EntPlayer::Update(float deltaTime)
{
	//Camera
	Ray r = _camera.GetProjection().ScreenToWorld(_rootCameraTransform, GameInstance::Instance().GetCursorPos());
	Vector3 cameraPos = r.origin + r.direction * Collision::IntersectRayPlane(r, GetWorldPosition() - Vector3(0.f, .5f, 0.f), Vector3(0.f, 1.f, 0.f));
	Vector3 targetPos = cameraPos + (cameraPos - GetWorldPosition());
	cameraPos += _camera.GetWorldTransform().GetForwardVector() * -5000.f;
	
	_targetTransform = Transform(targetPos + Vector3(-.02f, .3f, 0.f), Vector3(Maths::RadiansToDegrees(-Maths::ArcTan(1.f / Maths::SQRT2_F)), 45.f, 0.f));
	_camera.SetWorldPosition(cameraPos);

	//Movement
	const float sc45 = Maths::SQRT2_F / 2.f;

	//rotate by -45 degrees
	//float relVelX = _velocity.x * -sc45 + _velocity.z * -sc45;
	//float relVelZ = _velocity.z * -sc45 - _velocity.x * -sc45;
	
	static float relVelX = 0.f;
	static float relVelZ = 0.f;

	Vector2 input(GameInstance::Instance().GetAxisMoveRight(), GameInstance::Instance().GetAxisMoveForward());
	InputManager* im = GameInstance::Instance().GetGame()->GetContext().GetPtr<InputManager>();
	float iy = im->IsKeyDown(EKeycode::SPACE) ? 1.f : im->IsKeyDown(EKeycode::LSHIFT) ? -1.f : 0.f;

	if (input.LengthSquared() > 1.f)
		input.Normalise();
	
	Transform worldTransform = GetWorldTransform();
	Transform cameraT = _camera.GetWorldTransform();

	_velocity.y -= 9.8f * deltaTime;

	Vector3 dv;
	float amountForward = input.y * 10;
	if (amountForward)
		dv += cameraT.GetForwardVector() * (deltaTime * amountForward);

	float amountRight = input.x * 10;
	if (amountRight)
		dv += cameraT.GetRightVector() * (deltaTime * amountRight);

	_velocity.x += dv.x;
	_velocity.z += dv.z;


	Vector3 movement = _velocity * deltaTime;

	Transform desiredTransform = Transform(worldTransform.GetPosition() + movement, worldTransform.GetRotation(), worldTransform.GetScale());

	for (int _IT = 0; _IT < 20; ++_IT)
	{
		List<Pair<EOverlapResult, Vector3>> overlaps;
		GameInstance::Instance().world->GetOverlaps(overlaps, _COLLIDER, desiredTransform, this);
		if (overlaps.IsEmpty()) break;

		const Pair<EOverlapResult, Vector3>* largestOverlap = nullptr;
		float largestLengthSq = -1.f;
		for (const Pair<EOverlapResult, Vector3>& overlap : overlaps)
		{
			if (overlap.first == EOverlapResult::OVERLAPPING)
			{
				float lensq = overlap.second.LengthSquared();
				if (lensq > largestLengthSq)
				{
					largestOverlap = &overlap;
					largestLengthSq = lensq;
				}
			}
		}

		if (largestOverlap)
		{
			const Vector3& penetration = largestOverlap->second;
			Vector3 forbiddenDir = -penetration.Normalised();

			_velocity -= 1.5f * forbiddenDir * forbiddenDir.Dot(_velocity);

			desiredTransform.Move(penetration);
		}
	}

	if (Maths::AlmostEquals(_velocity.y, 0.f, 0.1f))
	{
		_velocity.x *= .998f;
		_velocity.z *= .998f;
	}

	SetWorldTransform(desiredTransform);
}

void EntPlayer::Render(RenderQueue& q) const
{
	if (_targetTexture)
	{
		RenderEntry& te = q.CreateEntry(ERenderChannels::UNLIT);
		te.AddSetColour();
		te.AddSetUVScale();
		te.AddSetUVOffset();
		te.AddSetTexture(*_targetTexture, 0);
		te.AddSetTransform(_targetTransform.GetTransformationMatrix());
		te.AddCommand(RCMDRenderMesh::PLANE);
	}
}
