#pragma once
#include "CollisionShape.hpp"
#include <ELMaths/Box.hpp>

class CollisionBox : public CollisionShape
{
public:
	CollisionBox(const Transform &transform = Transform()) : CollisionShape(transform) {}
	CollisionBox(const Box& box) : CollisionShape(Transform(box.centre, Rotation(), box.halfSize)) {}
	virtual ~CollisionBox() {}

	COLLISIONSHAPE_FUNCS(CollisionBox)

	void SetTransform(const Transform& transform)	{ _transform = transform; }
	void SetPosition(const Vector3& position)		{ _transform.SetPosition(position); }
	void SetRotation(const Rotation& rotation)		{ _transform.SetRotation(rotation); }
	void SetScale(const Vector3& scale)				{ _transform.SetScale(scale); }

	float GetMaximumScaledRadius() const override { return GetTransform().GetScale().Length(); }

	virtual bool IntersectsRay(const Ray&, RaycastResult&, const Transform& = Transform()) const override;
	virtual Vector3 GetNormalForPoint(const Vector3& point, const Transform& transform) const override;
	virtual Vector3 GetFarthestPointInDirection(const Vector3& axis, const Transform&) const override;
};
