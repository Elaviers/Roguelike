#include "Collider.hpp"
#include "Collision.hpp"
#include "CollisionBox.hpp"
#include "CollisionSphere.hpp"

bool Collider::IntersectsRay(const Transform& transform, const Ray& ray, RaycastResult& result) const
{
	if (CanCollideWithChannels(ray.channels))
		for (size_t i = 0; i < GetShapeCount(); ++i)
			if (GetShape(i).IntersectsRay(ray, result, transform))
				return true;

	return false;
}

bool Collider::Overlaps(const Transform& transform, const CollisionShape& otherShape, const Transform& otherShapeTransform) const
{
	for (size_t i = 0; i < _shapes.GetSize(); ++i)
	{
		const CollisionShape& shape = GetShape(i);

		if (shape.GetType() == CollisionType::BOX && otherShape.GetType() == CollisionType::BOX)
		{
			auto a = reinterpret_cast<const CollisionBox&>(shape);
			auto b = reinterpret_cast<const CollisionBox&>(otherShape);

			return Collision::BoxOverlapsBox(a.GetTransform() * transform, b.GetTransform() * otherShapeTransform);
		}

		if (shape.GetType() == CollisionType::SPHERE && otherShape.GetType() == CollisionType::SPHERE)
		{
			auto a = reinterpret_cast<const CollisionSphere&> (shape);
			auto b = reinterpret_cast<const CollisionSphere&> (otherShape);

			return Collision::SphereOverlapsSphere(a.GetTransform() * transform, a.GetRadius(), b.GetTransform() * otherShapeTransform, b.GetRadius());
		}

		if (shape.GetType() == CollisionType::SPHERE && otherShape.GetType() == CollisionType::BOX)
		{
			auto a = reinterpret_cast<const CollisionSphere&> (shape);
			auto b = reinterpret_cast<const CollisionBox&> (otherShape);

			return Collision::SphereOverlapsBox(a.GetTransform() * transform, a.GetRadius(), b.GetTransform() * otherShapeTransform);
		}

		if (shape.GetType() == CollisionType::BOX && otherShape.GetType() == CollisionType::SPHERE)
		{
			auto a = reinterpret_cast<const CollisionBox&> (shape);
			auto b = reinterpret_cast<const CollisionSphere&> (otherShape);

			return Collision::SphereOverlapsBox(b.GetTransform() * transform, b.GetRadius(), a.GetTransform() * transform);
		}
	}

	return false;
}

bool Collider::Overlaps(const Transform& transform, const Collider &other, const Transform &otherTransform) const
{
	if (CanCollideWith(other))
		for (size_t i = 0; i < other.GetShapeCount(); ++i)
			if (Overlaps(transform, other.GetShape(i), otherTransform))
				return true;

	return false;
}
