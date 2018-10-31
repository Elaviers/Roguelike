#pragma once
#include "Vector.h"

class Transform;

namespace Collision
{
	bool SphereOverlapsSphere(const Transform &t1, float r1, const Transform &t2, float r2);
	bool SphereOverlapsAABB(const Transform &t1, float r1, const Vector3 &min2, const Vector3 &max2);
	bool AABBOverlapsAABB(const Vector3 &min1, const Vector3 &max1, const Vector3 &min2, const Vector3 &max2);

}
