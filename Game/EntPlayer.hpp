#pragma once
#include <Engine/Entity.hpp>
#include <ELCore/SharedPointer.hpp>
#include <Engine/EntCamera.hpp>
#include <Engine/EntSkeletal.hpp>

class EntPlayer : public Entity
{
	Vector3 _velocity;

	EntSkeletal _mesh;

	EntCamera _camera;

	Transform _rootCameraTransform;

	SharedPointer<const Texture> _targetTexture;
	Transform _targetTransform;

	static Collider _COLLIDER;

	void _Jump();

	void _OnTransformChanged();
public:
	Entity_FUNCS(EntPlayer, 100);

	EntPlayer();
	virtual ~EntPlayer() {}

	void Init(const Context&);

	virtual void Update(float deltaTime) override;
	virtual void Render(RenderQueue&) const override;

	virtual const Collider* GetCollider() const override { return &_COLLIDER; };
};
