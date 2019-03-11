#include "Collider.h"
#include "ColliderAABB.h"
#include "ColliderSphere.h"
#include "Collision.h"

bool Collider::Overlaps(const Collider &other, const Transform &otherTransform, const Transform &_transform) const
{
	if (_type == ColliderType::AABB && other.GetType() == ColliderType::AABB)
	{
		auto a = reinterpret_cast<const ColliderAABB&>(*this);
		auto b = reinterpret_cast<const ColliderAABB&>(other);

		return Collision::AABBOverlapsAABB(a.min * _transform.GetTransformationMatrix(), a.max * _transform.GetTransformationMatrix(), b.min * otherTransform.GetTransformationMatrix(), b.max * otherTransform.GetTransformationMatrix());
	}

	if (_type == ColliderType::SPHERE && other.GetType() == ColliderType::SPHERE)
	{
		auto a = reinterpret_cast<const ColliderSphere&> (*this);
		auto b = reinterpret_cast<const ColliderSphere&> (other);

		return Collision::SphereOverlapsSphere(_transform, a.GetRadius(), otherTransform, b.GetRadius());
	}

	if (_type == ColliderType::SPHERE && other.GetType() == ColliderType::AABB)
	{
		auto a = reinterpret_cast<const ColliderSphere&> (*this);
		auto b = reinterpret_cast<const ColliderAABB&> (other);

		return Collision::SphereOverlapsAABB(_transform, a.GetRadius(), b.min * otherTransform.GetTransformationMatrix(), b.max * otherTransform.GetTransformationMatrix());
	}

	if (_type == ColliderType::AABB && other.GetType() == ColliderType::SPHERE)
	{
		auto a = reinterpret_cast<const ColliderAABB&> (*this);
		auto b = reinterpret_cast<const ColliderSphere&> (other);

		return Collision::SphereOverlapsAABB(otherTransform, b.GetRadius(), a.min * _transform.GetTransformationMatrix(), a.max * _transform.GetTransformationMatrix());
	}

	return false;
}
