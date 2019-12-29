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

/*
	Returns the farthest point in the minkowski difference in the given direction
*/
Vector3 Support(const CollisionShape& a, const Transform& tA, const CollisionShape& b, const Transform &tB, const Vector3& dir)
{
	return a.GetFarthestPointInDirection(dir, tA) - b.GetFarthestPointInDirection(-1.f * dir, tB);
}

/*
	Returns the result of (A X B) X C
*/
Vector3 TripleCross(const Vector3& a, const Vector3& b, const Vector3& c)
{
	return Vector3::Dot(c, a) * b - Vector3::Dot(c, b) * a;
}

constexpr const int GJK_MAX_ITERATIONS = 50;

bool GJK(const CollisionShape& shapeA, const Transform& tA, const CollisionShape& shapeB, const Transform& tB)
{
	//The simplex is a tetrahedron inside the minkowski difference
	Vector3 simplex[4];
	Vector3 d;
	int i = 0;
	for (int _iteration = 0; _iteration < GJK_MAX_ITERATIONS; ++_iteration)
	{
		switch (i)
		{
			case 0:
				//First point, search along A->B
				d = (shapeB.GetTransform().GetPosition() + tB.GetPosition()) - (shapeA.GetTransform().GetPosition() + tA.GetPosition());

				if (d.LengthSquared() == 0.f)
					d = Vector3(1, 0, 0);

				break;
			case 1:
				//The first point is past the origin
				//Look for the second support in the opposite direction
				d *= -1.f;
				break;
			case 2:
			{
				//The simplex is a line with points on either side of the origin
				//Look for the third support perpendicular to the line, facing the origin

				Vector3 ba = simplex[0] - simplex[1];
				Vector3 b0 = simplex[1] * -1.f;

				//(BA X B0) X BA
				d = TripleCross(ba, b0, ba);
				break;
			}
			case 3:
			{
				//The simplex is a triangle that contains the origin in its respective space
				//Look for the fourth support along the normal which is facing the origin

				Vector3 dc = simplex[1] - simplex[2];
				Vector3 db = simplex[0] - simplex[2];

				d = Vector3::Cross(dc, db);

				//If cross product is not facing origin, flip it
				if (Vector3::Dot(d, -1.f * simplex[2]) < 0.f)
					d *= -1.f;

				break;
			}
			case 4:
			{
				//The simplex is a tetrahedron whose base (points 0, 1, 2) is on the opposite side of the origin from its peak (point 3)
				//Check if the tetrahedron contains the origin by checking the dot products of the normals of the 3 faces related to the peak
				//The base's normal doesn't have to be checked as it is on the opposite side of the origin from the peak

				Vector3 d0 = simplex[3] * -1.f;

				//Edges
				Vector3 dc = simplex[2] - simplex[3];
				Vector3 db = simplex[1] - simplex[3];
				
				Vector3 bcd = Vector3::Cross(dc, db);
				if (Vector3::Dot(bcd, d0) > 0.f)
				{
					//Remove point A (0)
					simplex[0] = simplex[1];
					simplex[1] = simplex[2];
					simplex[2] = simplex[3];
					i = 3;

					d = bcd;
					break;
				}

				Vector3 da = simplex[0] - simplex[3];

				Vector3 abd = Vector3::Cross(db, da);
				if (Vector3::Dot(abd, d0) > 0.f)
				{
					//Remove point C (2)
					simplex[2] = simplex[3];
					i = 3;

					d = abd;
					break;
				}

				Vector3 acd = Vector3::Cross(da, dc);
				if (Vector3::Dot(acd, d0) > 0.f)
				{
					//Remove point B (1)
					simplex[1] = simplex[2];
					simplex[2] = simplex[3];
					i = 3;

					d = acd;
					break;
				}

				//The origin is behind the three face normals, collision found
				return true;
			}
		}

		simplex[i] = Support(shapeA, tA, shapeB, tB, d);

		//Check if the new point is past the origin
		if (Vector3::Dot(simplex[i], d) < 0.f)
			return false;

		++i;
	}

	//Failsafe
	return true;
}

bool Collider::Overlaps(const Transform &transform, const Collider& other, const Transform &otherTransform) const
{
	for (size_t i = 0; i < GetShapeCount(); ++i)
		for (size_t j = 0; j < GetShapeCount(); ++j)
			if (GJK(GetShape(i), transform, other.GetShape(j), otherTransform))
				return true;

	return false;
}
