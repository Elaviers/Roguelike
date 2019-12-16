#pragma once
#include "Ray.hpp"
#include "RaycastResult.hpp"
#include "Transform.hpp"

enum class CollisionType
{
	BOX,
	CAPSULE,
	CONVEX,
	SPHERE
};

class CollisionShape
{
protected:
	const CollisionType _type;
	Transform _transform;

	CollisionShape(CollisionType type, const Transform& transform) : _type(type), _transform(transform) {}

public:
	virtual ~CollisionShape() {}

	const Transform& GetTransform() const { return _transform; }

	void SetTransform(const Transform& transform) { _transform = transform; }
	void SetPosition(const Vector3& position) { _transform.SetPosition(position); }
	void SetRotation(const Vector3& rotation) { _transform.SetRotation(rotation); }
	void SetScale(const Vector3& scale) { _transform.SetScale(scale); }

	const CollisionType& GetType() const { return _type; }

	virtual bool IntersectsRay(const Ray&, RaycastResult&, const Transform& transform) const = 0;

	virtual Vector3 GetNormalForPoint(const Vector3& point, const Transform& transform) const = 0;
};
