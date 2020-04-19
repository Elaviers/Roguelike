#pragma once
#include "CollisionShape.hpp"

class CollisionSphere : public CollisionShape
{
	float _radius;

public:
	CollisionSphere(float radius = 0.f, Vector3 translation = Vector3()) : CollisionShape(Transform(translation)), _radius(radius) {}
	virtual ~CollisionSphere() {}

	virtual CollisionShape* Clone() const override { return new CollisionSphere(*this); }

	void SetRadius(float radius) { _radius = radius; }
	void SetPosition(const Vector3& translation) { _transform.SetPosition(translation); }

	float GetRadius() const { return _radius; }

	virtual float GetMaximumScaledRadius() const override { return _radius * Maths::Min(GetTransform().GetScale().GetData(), 3); }

	virtual bool IntersectsRay(const Ray&, RaycastResult&, const Transform& = Transform()) const override;
	virtual Vector3 GetNormalForPoint(const Vector3& point, const Transform& transform) const override;
	virtual Vector3 GetFarthestPointInDirection(const Vector3& axis, const Transform&) const override;
};
