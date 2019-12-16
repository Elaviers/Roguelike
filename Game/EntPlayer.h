#pragma once
#include <Engine/Entity.hpp>
#include <Engine/EntCamera.hpp>
#include <Engine/EntSkeletal.hpp>

class EntPlayer : public Entity
{
	Vector3 _velocity;

	EntSkeletal _mesh;

	Entity _cameraPivot;
	EntCamera _camera;

	static Collider _COLLIDER;

public:
	Entity_FUNCS(EntPlayer, 100);

	EntPlayer();
	virtual ~EntPlayer() {}

	void Update(float deltaTime);
};
