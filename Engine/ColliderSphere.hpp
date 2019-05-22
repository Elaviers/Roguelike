#pragma once
#include "Collider.hpp"

class ColliderSphere : public Collider
{
	float _radius;

public:
	ColliderSphere(float radius = 0.f) : Collider(ColliderType::SPHERE), _radius(radius) {}
	virtual ~ColliderSphere() {}

	inline float GetRadius() const { return _radius; }

	virtual bool IntersectsRay(const Ray&, RaycastResult&, const Transform& = Transform()) const override;
};
