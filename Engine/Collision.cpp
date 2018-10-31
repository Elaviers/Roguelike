#include "Collision.h"
#include "Transform.h"

namespace Collision
{
	bool SphereOverlapsSphere(const Transform &t1, float r1, const Transform &t2, float r2)
	{
		float d = (t1.Position() - t2.Position()).LengthSquared();
		float maxD = r1 * r1 + r2 * r2;

		return d <= maxD;
	}

	bool SphereOverlapsAABB(const Transform &t1, float r1, const Vector3 &min2, const Vector3 &max2)
	{
		Vector3 closestPoint = Vector3(
			Maths::Clamp(min2[0], max2[0], t1.Position()[0]),
			Maths::Clamp(min2[1], max2[1], t1.Position()[1]),
			Maths::Clamp(min2[2], max2[2], t1.Position()[2]));

		float dist = (t1.Position() - closestPoint).LengthSquared();
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
