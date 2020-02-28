#pragma once
#include "GizmoComponent.hpp"
#include <Engine/Collider.hpp>
#include <Engine/CollisionBox.hpp>
#include <Engine/FunctionPointer.hpp>

/*
	GizmoAxis

	Line thingy
*/

class GizmoAxis : public GizmoComponent
{
	Vector3 _grabOffset;

	Vector3 _fv;
	float _length;

	Collider _collider;
	CollisionBox* _collisionBox;

	Setter<const Vector3&> _move;

protected:
	virtual void _OnTransformChanged() override;

public:
	GizmoAxis(const Colour& colour, const Setter<const Vector3&>& move) : 
		GizmoComponent(colour),
		_length(1.f), 
		_collider(ECollisionChannels::EDITOR),
		_move(move)
	{
		_collisionBox = &_collider.AddShape(CollisionBox(Transform(Vector3(0.f, 0.5f, 0.f), Rotation(), Vector3(0.1f, 1.f, 0.1f))));
	}

	virtual ~GizmoAxis() {}

	virtual void Draw() const override;
	virtual void Update(const Ray &mouseRay, float &maxT) override;
};
