#pragma once
#include "Collider.hpp"
#include "ColliderSphere.hpp"

#undef min
#undef max

class ColliderAABB : public Collider
{
public:
	ColliderAABB(Vector3 min = Vector3(), Vector3 max = Vector3()) : Collider(ColliderType::AABB), min(min), max(max) {}
	virtual ~ColliderAABB() {}

	Vector3 min;
	Vector3 max;

	virtual bool IntersectsRay(const Ray&, RaycastResult&, const Transform& = Transform()) const override;
};
