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

	void _Jump();

	bool _TryMovement(const Transform &wt, const Vector3 &movement, float testYOffset);

public:
	Entity_FUNCS(EntPlayer, 100);

	EntPlayer() {}
	virtual ~EntPlayer() {}

	void Init(const Context&);

	void Update(float deltaTime);

	virtual const Collider* GetCollider() const override { return &_COLLIDER; };
};
