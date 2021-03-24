#include "OPlayer.hpp"
#include "Game.hpp"
#include "GameInstance.hpp"
#include <ELCore/Context.hpp>
#include <ELPhys/Collision.hpp>
#include <ELPhys/CollisionBox.hpp>
#include <ELPhys/CollisionCapsule.hpp>
#include <ELSys/InputManager.hpp>
#include <Engine/ModelManager.hpp>

void OPlayer::_OnTransformChanged()
{
	WorldObject::_OnTransformChanged();
	_physics->SetTransform(GetAbsoluteTransform());
	_rootCameraTransform = GetAbsoluteTransform();
	_rootCameraTransform.SetRotation(_camera->GetAbsoluteRotation());
	_rootCameraTransform.Move(_camera->GetForwardVector() * -5000.f);
}

OPlayer::OPlayer(World& world) : 
	WorldObject(world),
	_mesh(world.CreateObject<OSkeletal>()),
	_camera(world.CreateObject<OCamera>()),
	_physics(world.CreatePhysicsBody(this))
{
	_mesh->SetParent(this, false);
	_mesh->SetCollisionEnabled(false);
	_mesh->SetModel("capsule", *world.GetContext());
	_mesh->SetMaterial("white", *world.GetContext());
	_mesh->SetRelativeScale(Vector3(.5f, .5f, .5f));

	_camera->SetParent(this, false);
	_camera->GetProjection().SetType(EProjectionType::ORTHOGRAPHIC);
	_camera->SetRelativeRotation(Vector3(Maths::RadiansToDegrees(-Maths::ArcTan(1.f / Maths::SQRT2_F)), 45.f, 0.f));
	_camera->GetProjection().SetNearFar(0.f, 10000.f);
	_camera->GetProjection().SetOrthographicScale(64.f);

	_physics->Collision() = Collider(ECollisionChannels::PLAYER, CollisionCapsule(1.f, .5f));
	_physics->SetRotationLocked(true);

	GameInstance::Instance().SetActiveCamera(_camera);

	world.GetContext()->GetPtr<InputManager>()->BindKeyDown(EKeycode::SPACE, Callback(*this, &OPlayer::_Jump));

	_targetTexture = world.GetContext()->GetPtr<TextureManager>()->Get("iso/targ_temp", *world.GetContext());
}

OPlayer::OPlayer(const OPlayer& other) :
	WorldObject(other)
{
	//todo
}

void OPlayer::_Jump()
{
	_physics->SetVelocity(_physics->GetVelocity() + Vector3(0.f, 5.f, 0.f));
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

#include <Engine/imgui/imgui.h>

void OPlayer::Update(float deltaTime)
{
	SetAbsoluteTransform(_physics->GetTransform());

	//Camera
	Ray r = _camera->GetProjection().ScreenToWorld(_rootCameraTransform, GameInstance::Instance().GetCursorPos());
	Vector3 cameraPos = r.origin + r.direction * Collision::IntersectRayPlane(r, GetAbsolutePosition() - Vector3(0.f, .5f, 0.f), Vector3(0.f, 1.f, 0.f));
	Vector3 targetPos = cameraPos + (cameraPos - GetAbsolutePosition());
	cameraPos += _camera->GetAbsoluteTransform().GetForwardVector() * -5000.f;
	
	_targetTransform = Transform(targetPos + Vector3(-.02f, .3f, 0.f), Vector3(Maths::RadiansToDegrees(-Maths::ArcTan(1.f / Maths::SQRT2_F)), 45.f, 0.f));
	_camera->SetAbsolutePosition(cameraPos);

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
	
	const Transform& worldTransform = GetAbsoluteTransform();
	const Transform& cameraT = _camera->GetAbsoluteTransform();

	Vector3 dv;
	float amountForward = input.y * 10;
	if (amountForward)
		dv += cameraT.GetForwardVector() * (deltaTime * amountForward);

	float amountRight = input.x * 10;
	if (amountRight)
		dv += cameraT.GetRightVector() * (deltaTime * amountRight);

	_physics->SetVelocity(_physics->GetVelocity() + dv);


	//debug window
	static bool playerDebug = false;
	
	ImGui::SetNextWindowSize(ImVec2(200, 100), ImGuiCond_FirstUseEver);
	if (ImGui::Begin(CSTR("[", GetUID(), "](Player) \"", GetName().ToString(), "\""), &playerDebug))
	{
		Vector3 pos = GetAbsolutePosition();

		if (ImGui::DragFloat3("Position", &pos[0]))
			SetAbsolutePosition(pos);

		ImGui::InputFloat3("TargetPos", &targetPos[0], "%.3f", ImGuiInputTextFlags_ReadOnly);

		ImGui::End();
	}
}

void OPlayer::Render(RenderQueue& q) const
{
	if (_targetTexture)
	{
		RenderEntry& te = q.CreateEntry(ERenderChannels::UNLIT);
		te.AddSetColour();
		te.AddSetUVScale();
		te.AddSetUVOffset();
		te.AddSetTexture(*_targetTexture, 0);
		te.AddSetTransform(_targetTransform.GetMatrix());
		te.AddCommand(RCMDRenderMesh::PLANE);
	}
}
