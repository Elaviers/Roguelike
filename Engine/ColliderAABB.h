#pragma once
#include "Collider.h"
#include "ColliderSphere.h"

class ColliderAABB : public Collider
{
public:
	ColliderAABB(const GameObject *parent) : Collider(parent) {}
	virtual ~ColliderAABB() {}

	Vector3 min;
	Vector3 max;

	virtual bool IntersectsRay(const Ray&, RaycastResult&) const override;
	
	virtual bool Overlaps(const ColliderAABB&) const override;
	virtual bool Overlaps(const ColliderSphere &other) const override { return other.Overlaps(*this); }

	friend virtual bool ColliderSphere::Overlaps(const ColliderAABB&) const;
};
