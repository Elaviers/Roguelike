#include "Collider.hpp"
#include "ColliderBox.hpp"
#include "ColliderSphere.hpp"
#include "Collision.hpp"

bool Collider::Overlaps(const Collider &other, const Transform &otherTransform, const Transform &_transform) const
{
	if (!CanCollideWith(other)) return false;

	if (_type == ColliderType::BOX && other.GetType() == ColliderType::BOX)
	{
		auto a = reinterpret_cast<const ColliderBox&>(*this);
		auto b = reinterpret_cast<const ColliderBox&>(other);

		return Collision::BoxOverlapsBox(a.transform * _transform, a.extent, b.transform * otherTransform, b.extent);
	}

	if (_type == ColliderType::SPHERE && other.GetType() == ColliderType::SPHERE)
	{
		auto a = reinterpret_cast<const ColliderSphere&> (*this);
		auto b = reinterpret_cast<const ColliderSphere&> (other);

		return Collision::SphereOverlapsSphere(_transform, a.GetRadius(), otherTransform, b.GetRadius());
	}

	if (_type == ColliderType::SPHERE && other.GetType() == ColliderType::BOX)
	{
		auto a = reinterpret_cast<const ColliderSphere&> (*this);
		auto b = reinterpret_cast<const ColliderBox&> (other);

		return Collision::SphereOverlapsBox(_transform, a.GetRadius(), b.transform * otherTransform, b.extent);
	}

	if (_type == ColliderType::BOX && other.GetType() == ColliderType::SPHERE)
	{
		auto a = reinterpret_cast<const ColliderBox&> (*this);
		auto b = reinterpret_cast<const ColliderSphere&> (other);

		return Collision::SphereOverlapsBox(otherTransform, b.GetRadius(), a.transform * _transform, a.extent);
	}

	return false;
}
