#pragma once
#include "Ray.hpp"
#include "Transform.hpp"


enum class ColliderType
{
	CUSTOM,
	AABB,
	SPHERE
};

struct RaycastResult;

class Collider
{
protected:
	const ColliderType _type;

	Collider(ColliderType type) : _type(type) {}

public:
	virtual ~Collider() {}

	inline const ColliderType& GetType() const { return _type; }

	virtual bool IntersectsRay(const Ray&, RaycastResult&, const Transform& _transform) const = 0;

	bool Overlaps(const Collider &other, const Transform &otherTransform, const Transform &_transform) const;
};
