#pragma once
#include "Collider.h"

class ColliderSphere : public Collider
{
	float _radius;

public:
	ColliderSphere(const GameObject *parent, float radius = 0.f) : Collider(parent), _radius(radius) {}
	virtual ~ColliderSphere() {}

	virtual bool IntersectsRay(const Ray&, RaycastResult&) const override;

	virtual bool Overlaps(const ColliderAABB&) const override;
	virtual bool Overlaps(const ColliderSphere&) const override;
};
