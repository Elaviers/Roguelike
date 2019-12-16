#pragma once
#include "CollisionShape.hpp"
#include "Box.hpp"

class CollisionBox : public CollisionShape
{
public:
	CollisionBox(const Transform &transform = Transform()) : CollisionShape(CollisionType::BOX, transform) {}
	CollisionBox(const Box& box) : CollisionShape(CollisionType::BOX, Transform(box.centre, Rotation(), box.extent)) {}
	virtual ~CollisionBox() {}

	virtual bool IntersectsRay(const Ray&, RaycastResult&, const Transform& = Transform()) const override;

	virtual Vector3 GetNormalForPoint(const Vector3& point, const Transform& transform) const override;

	Vector3 GetPointWithHighestDot(const Vector3& axis, const Transform&) const;


	//
	CollisionBox& operator=(const CollisionBox& other)
	{
		_transform = other._transform;
		return *this;
	}
};
