#include "Collider.h"
#include "ColliderAABB.h"
#include "ColliderSphere.h"
#include "Collision.h"

bool Collider::Overlaps(const Collider &other, const Transform &otherTransform, const Transform &transform) const
{
	if (_type == ColliderType::AABB && other.GetType() == ColliderType::AABB)
	{
		auto a = reinterpret_cast<const ColliderAABB&>(*this);
		auto b = reinterpret_cast<const ColliderAABB&>(other);

		return Collision::AABBOverlapsAABB(transform.Position() + a.min, transform.Position() + a.max, otherTransform.Position() + b.min, otherTransform.Position() + b.max);
	}

	if (_type == ColliderType::SPHERE && other.GetType() == ColliderType::SPHERE)
	{
		auto a = reinterpret_cast<const ColliderSphere&> (*this);
		auto b = reinterpret_cast<const ColliderSphere&> (other);

		return Collision::SphereOverlapsSphere(transform, a.GetRadius(), otherTransform, b.GetRadius());
	}

	if (_type == ColliderType::SPHERE && other.GetType() == ColliderType::AABB)
	{
		auto a = reinterpret_cast<const ColliderSphere&> (*this);
		auto b = reinterpret_cast<const ColliderAABB&> (other);

		return Collision::SphereOverlapsAABB(transform, a.GetRadius(), otherTransform.Position() + b.min, otherTransform.Position() + b.max);
	}

	if (_type == ColliderType::AABB && other.GetType() == ColliderType::SPHERE)
	{
		auto a = reinterpret_cast<const ColliderAABB&> (*this);
		auto b = reinterpret_cast<const ColliderSphere&> (other);

		return Collision::SphereOverlapsAABB(otherTransform, b.GetRadius(), transform.Position() + a.min, transform.Position() + a.max);
	}

	return false;
}
