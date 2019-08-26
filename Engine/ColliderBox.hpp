#pragma once
#include "Collider.hpp"
#include "ColliderSphere.hpp"
#include "Box.hpp"

#undef min
#undef max

class ColliderBox : public Collider
{
public:
	ColliderBox(CollisionChannel channels, const Vector3 &extent = Vector3(), const Transform &transform = Transform()) : Collider(ColliderType::BOX, channels), extent(extent), transform(transform) {}
	ColliderBox(CollisionChannel channels, const Box& box) : Collider(ColliderType::BOX, channels), extent(box.extent), transform(box.centre) {}
	virtual ~ColliderBox() {}

	Vector3 extent;
	Transform transform;

	virtual bool IntersectsRay(const Ray&, RaycastResult&, const Transform& = Transform()) const override;
};
