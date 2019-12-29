#pragma once
#include "Ray.hpp"
#include "RaycastResult.hpp"
#include "Transform.hpp"

#define COLLISIONSHAPE_FUNCS(TYPENAME) virtual CollisionShape* Clone() const override { return new TYPENAME(*this); }

class CollisionShape
{
protected:
	Transform _transform;

	CollisionShape(const Transform& transform) : _transform(transform) {}

public:
	virtual ~CollisionShape() {}

	virtual CollisionShape* Clone() const = 0;

	const Transform& GetTransform() const { return _transform; }

	virtual bool IntersectsRay(const Ray&, RaycastResult&, const Transform& transform) const = 0;
	virtual Vector3 GetNormalForPoint(const Vector3& point, const Transform& transform) const = 0;
	virtual Vector3 GetFarthestPointInDirection(const Vector3& axis, const Transform&) const = 0;
};
