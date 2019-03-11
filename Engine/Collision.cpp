#include "Collision.h"
#include "Transform.h"

namespace Collision
{
	bool SphereOverlapsSphere(const Transform &t1, float r1, const Transform &t2, float r2)
	{
		float d = (t1.GetPosition() - t2.GetPosition()).LengthSquared();
		float maxD = r1 * r1 + r2 * r2;

		return d <= maxD;
	}

	bool SphereOverlapsAABB(const Transform &t1, float r1, const Vector3 &min2, const Vector3 &max2)
	{
		Vector3 closestPoint = Vector3(
			Utilities::Clamp(t1.GetPosition()[0], min2[0], max2[0]),
			Utilities::Clamp(t1.GetPosition()[1], min2[1], max2[1]),
			Utilities::Clamp(t1.GetPosition()[2], min2[2], max2[2]));

		float dist = (t1.GetPosition() - closestPoint).LengthSquared();
		if (dist <= r1 * r1)
			return true;

		return false;
	}

	bool SphereOverlapsAABB(const Vector3 &v, float r1, const Vector3 &min2, const Vector3 &max2)
	{
		Vector3 closestPoint = Vector3(
			Utilities::Clamp(v[0], min2[0], max2[0]),
			Utilities::Clamp(v[1], min2[1], max2[1]),
			Utilities::Clamp(v[2], min2[2], max2[2]));

		float dist = (v - closestPoint).LengthSquared();
		if (dist <= r1 * r1)
			return true;

		return false;
	}

	bool AABBOverlapsAABB(const Vector3 &min1, const Vector3 &max1, const Vector3 &min2, const Vector3 &max2)
	{
		if (max2[0] < min1[0] || max2[1] < min1[1] || max2[2] < min1[2] ||
			min2[0] > max1[0] || min2[1] > max1[1] || min2[2] > max1[2])
			return false;

		return true;
	}

}
