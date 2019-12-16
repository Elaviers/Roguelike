#pragma once
#include "CollisionShape.hpp"

class CollisionSphere : public CollisionShape
{
	float _radius;

public:
	CollisionSphere(float radius = 0.f, Vector3 translation = Vector3()) : CollisionShape(CollisionType::SPHERE, Transform(translation)), _radius(radius) {}
	virtual ~CollisionSphere() {}

	float GetRadius() const { return _radius; }
	void SetRadius(float radius) { _radius = radius; }

	const Vector3& GetTranslation() const { return _transform.GetPosition(); }
	void SetTranslation(const Vector3& translation) { _transform.SetPosition(translation); }

	virtual bool IntersectsRay(const Ray&, RaycastResult&, const Transform& = Transform()) const override;

	virtual Vector3 GetNormalForPoint(const Vector3& point, const Transform& transform) const override;

	Vector3 GetPointWithHighestDot(const Vector3& axis, const Transform&) const;

	//
	CollisionSphere& operator=(const CollisionSphere& other)
	{
		_transform = other._transform;
		_radius = other._radius;
		return *this;
	}
};
