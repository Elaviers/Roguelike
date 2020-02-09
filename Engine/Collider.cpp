#include "Collider.hpp"
#include "Collision.hpp"
#include "CollisionBox.hpp"
#include "CollisionSphere.hpp"
#include "LineSegment.h"

bool Collider::IntersectsRay(const Transform& transform, const Ray& ray, RaycastResult& result) const
{
	if (CanCollideWithChannels(ray.channels))
		for (size_t i = 0; i < GetShapeCount(); ++i)
			if (GetShape(i).IntersectsRay(ray, result, transform))
				return true;

	return false;
}

/*
	Returns the result of (A X B) X C
*/
Vector3 TripleCross(const Vector3& a, const Vector3& b, const Vector3& c)
{
	return Vector3::Dot(c, a) * b - Vector3::Dot(c, b) * a;
}

constexpr const int GJK_MAX_ITERATIONS = 50;

//GJK will terminate if the dot product between a point and an edge normal is lower than this
constexpr const float GJK_EDGE_TOLERANCE = 0.001f;

//GJKDist will terminate if a the dot product between an existing point and the direction and the dot product between the new point and the direction is lower than this
constexpr const double GJK_TOLERANCE = 1e-10;

//#define GJK_DEBUG 1
#define GJK_MATHEMATICA_DEBUG 0

#if GJK_MATHEMATICA_DEBUG
inline void MathematicaDebugState(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d, const Vector3& closestPoint)
{
	Debug::PrintLine(CSTR("{{", a, "},{", b, "},{", c, "},{", d, "},{", closestPoint, "}},"));
}
#else
__forceinline void MathematicaDebugState(const Vector3& a, const Vector3& b, const Vector3& c, const Vector3& d, const Vector3& closestPoint) {}
#endif

bool GJK(const CollisionShape& shapeA, const Transform& tA, const CollisionShape& shapeB, const Transform& tB, const LineSegment* pLineA = nullptr)
{
	//The simplex is a tetrahedron inside the minkowski difference
	Vector3 simplex[4];
	Vector3 dir;
	int i = 0;
	for (int _iteration = 0; _iteration < GJK_MAX_ITERATIONS; ++_iteration)
	{
		switch (i)
		{
			case 0:
				//First point, search along A->B
				dir = (shapeB.GetTransform().GetPosition() + tB.GetPosition()) - (shapeA.GetTransform().GetPosition() + tA.GetPosition());

				if (dir.LengthSquared() == 0.f)
					dir = Vector3(1, 0, 0);

				break;
			case 1:
				//The first point is past the origin
				//Look for the second support in the opposite direction
				dir *= -1.f;
				break;
			case 2:
			{
				//The simplex is a line with points on either side of the origin
				//Look for the third support perpendicular to the line, facing the origin

				Vector3 ba = simplex[0] - simplex[1];
				Vector3 b0 = -simplex[1];

				//(BA X B0) X BA
				dir = TripleCross(ba, b0, ba);
				break;
			}
			case 3:
			{
				//The simplex is a triangle that contains the origin in its respective space
				//Look for the fourth support along the normal which is facing the origin

				Vector3 cb = simplex[1] - simplex[2];
				Vector3 ca = simplex[0] - simplex[2];

				dir = Vector3::Cross(cb, ca);

				//If cross product is not facing origin, flip it
				if (Vector3::Dot(dir, -simplex[2]) < 0.f)
					dir *= -1.f;

				break;
			}
			case 4:
			{
				//The simplex is a tetrahedron whose base (points 0, 1, 2) is on the opposite side of the origin from its peak (point 3)
				//Check if the tetrahedron contains the origin by checking the dot products of the normals of the 3 faces related to the peak
				//The base's normal doesn't have to be checked as it is on the opposite side of the origin from the peak

				Vector3 d0 = -simplex[3];

				//Edges
				Vector3 dc = simplex[2] - simplex[3];
				Vector3 db = simplex[1] - simplex[3];
				
				Vector3 bcd = Vector3::Cross(dc, db);
				float dot = Vector3::Dot(bcd, d0);
				if (Maths::AlmostEqual(dot, 0.f, GJK_EDGE_TOLERANCE))
					return false;

				if (dot > 0.f)
				{
					//Remove point A (0)
					simplex[0] = simplex[1];
					simplex[1] = simplex[2];
					simplex[2] = simplex[3];
					i = 3;

					dir = bcd;
					break;
				}

				Vector3 da = simplex[0] - simplex[3];

				Vector3 abd = Vector3::Cross(db, da);
				dot = Vector3::Dot(abd, d0);
				if (Maths::AlmostEqual(dot, 0.f, GJK_EDGE_TOLERANCE))
					return false;

				if (dot > 0.f)
				{
					//Remove point C (2)
					simplex[2] = simplex[3];
					i = 3;

					dir = abd;
					break;
				}

				Vector3 acd = Vector3::Cross(da, dc);
				dot = Vector3::Dot(acd, d0);
				if (Maths::AlmostEqual(dot, 0.f, GJK_EDGE_TOLERANCE))
					return false;

				if (dot > 0.f)
				{
					//Remove point B (1)
					simplex[1] = simplex[2];
					simplex[2] = simplex[3];
					i = 3;

					dir = acd;
					break;
				}

				//The origin is behind the three face normals, collision found
				return true;
			}
		}

		//Find new support point
		if (pLineA)
		{
			Vector3 farthestLinePoint = Vector3::Dot(dir, pLineA->end - pLineA->start) > 0.f ? pLineA->end : pLineA->start;

			simplex[i] = (farthestLinePoint + shapeA.GetFarthestPointInDirection(dir, tA)) - shapeB.GetFarthestPointInDirection(-dir, tB);
		}
		else
			simplex[i] = shapeA.GetFarthestPointInDirection(dir, tA) - shapeB.GetFarthestPointInDirection(-dir, tB);

		//Fail if the new point did not go past the origin
		if (Vector3::Dot(simplex[i], dir) < 0.f)
			return false;

		++i;
	}

	//Failsafe
	return true;
}

bool Collider::Overlaps(const Transform &transform, const Collider& other, const Transform &otherTransform, const LineSegment* lineA) const
{
	if (CanCollideWith(other))
		for (size_t i = 0; i < GetShapeCount(); ++i)
			for (size_t j = 0; j < GetShapeCount(); ++j)
				if (GJK(GetShape(i), transform, other.GetShape(j), otherTransform, lineA))
					return true;

	return false;
}

inline Vector3 ClosestPointToOriginOnPlane(const Vector3& planePoint, const Vector3& planeNormal)
{
	Debug::Assert(Maths::AlmostEqual(planeNormal.LengthSquared(), 1.f, .001f), "ClosestPointToOriginOnPlane requires a normalised normal vector...");

	return Vector3::Dot(planePoint, planeNormal) * planeNormal;
}

inline Vector3 ClosestPointToOriginOnLineSegment(const Vector3& a, const Vector3& ab)
{
	float length = ab.Length();

	if (length == 0.f)
		return a;

	Vector3 d = ab / length;
	float dot = Vector3::Dot(-a, d);

	if (dot <= 0.f) return a;
	if (dot >= length) return a + ab;
	return a + d * dot;
}

Vector3 ClosestPointToOriginOnTriangle(const Vector3& a, const Vector3& b, const Vector3& c)
{
	if (a == b) return ClosestPointToOriginOnLineSegment(a, c-a);
	if (a == c || b == c) return ClosestPointToOriginOnLineSegment(a, b-a);

	Vector3 ab = b - a;
	Vector3 bc = c - b;
	Vector3 ca = a - c;

	//A.-N[AB]
	if (Vector3::Dot(a, TripleCross(ab, ca, ab)) <= 0.f)
	{
		if (Vector3::Dot(a, -ab) <= 0.f)				//A.BA (outside a on ab)
		{
			if (Vector3::Dot(a, ca) <= 0.f) return a;	//A.CA (outside a on ca)
			return ClosestPointToOriginOnLineSegment(c, ca);
		}

		if (Vector3::Dot(b, ab) <= 0.f)					//B.AB (outside b on ab)
		{
			if (Vector3::Dot(b, -bc) <= 0.f) return b;	//B.CB (outside b on bc)
			return ClosestPointToOriginOnLineSegment(b, bc);
		}

		return ClosestPointToOriginOnLineSegment(a, ab);
	}

	//B.-N[BC]
	if (Vector3::Dot(b, TripleCross(bc, ab, bc)) <= 0.f)
	{
		if (Vector3::Dot(b, -bc) <= 0.f) return b;		//B.CB (outside b on bc)

		if (Vector3::Dot(c, bc) <= 0.f)					//B.BC (outside c on bc)
		{
			if (Vector3::Dot(c, -ca) <= 0.f) return c;	//C.AC (outside c on ca)

			return ClosestPointToOriginOnLineSegment(c, ca);
		}

		return ClosestPointToOriginOnLineSegment(b, bc);
	}

	//C.-N[CA]
	if (Vector3::Dot(c, TripleCross(ca, -ab, ca)) <= 0.f)
	{
		if (Vector3::Dot(a, ca) <= 0.f) return a;		//A.CA (outside a on ca)
		if (Vector3::Dot(c, -ca) <= 0.f) return c;		//C.AC (outside c on ca)
		return ClosestPointToOriginOnLineSegment(c, ca);
	}

	return ClosestPointToOriginOnPlane(a, Vector3::Cross(ab, bc).Normalised());
}

inline Vector2 Cartesian2Barycentric(const Vector3& p, const Vector3& a, const Vector3& b)
{
	Vector2 result;
	result[0] = (p - b).Length() / (a - b).Length();
	result[1] = 1.f - result[0];
	return result;
}

Vector3 Cartesian2Barycentric(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c)
{
	if (a == b)
	{
		Vector2 bary2D = Cartesian2Barycentric(p, a, c);
		return Vector3(bary2D[0], 0.f, bary2D[1]);
	}

	if (a == c || b == c)
	{
		Vector2 bary2D = Cartesian2Barycentric(p, a, b);
		return Vector3(0.f, bary2D[0], bary2D[1]);
	}

	Vector3 result;
	Vector3 ab = b - a, ac = c - a, ap = p - a;
	float ab_ab = Vector3::Dot(ab, ab);
	float ab_ac = Vector3::Dot(ab, ac);
	float ac_ac = Vector3::Dot(ac, ac);
	float ap_ab = Vector3::Dot(ap, ab);
	float ap_ac = Vector3::Dot(ap, ac);
	float denom = ab_ab * ac_ac - ab_ac * ab_ac;

	if (denom == 0.f)
	{
		Vector2 bary2D = Cartesian2Barycentric(p, a, b);
		return Vector3(bary2D[0], bary2D[1], 0.f);
	}

	result[1] = (ac_ac * ap_ab - ab_ac * ap_ac) / denom;
	result[2] = (ab_ab * ap_ac - ab_ac * ap_ab) / denom;
	result[0] = 1.0f - result[1] - result[2];
	return result;
}

//Returns vector perpendicular to the two sides in the direction of dir
inline Vector3 GetNormalForFace(const Vector3& side1, const Vector3& side2, const Vector3& dir)
{
	Vector3 result = Vector3::Cross(side1, side2);

	if (Vector3::Dot(result, dir) < 0.f)
		return -result;

	return result;
}

inline float CoolDot(const Vector3& side1, const Vector3& side2, const Vector3& dirFromCentroid, const Vector3& dirFromOrigin)
{
	return Vector3::Dot(GetNormalForFace(side1, side2, dirFromCentroid), dirFromOrigin);
}

#include "Engine.hpp"
#include "DebugManager.hpp"
#include "DebugLine.hpp"

float GJKDist(
	const CollisionShape& shapeA, const Transform& tA, 
	const CollisionShape& shapeB, const Transform& tB, 
	Vector3& pointA, Vector3& pointB, const LineSegment* pLineA)
{
#if GJK_DEBUG
	Debug::PrintLine("STARTING GJKDIST...");
#endif

	struct SimplexPoint
	{
		Vector3 aSupport;
		Vector3 bSupport;
		Vector3 difference;

		SimplexPoint() {}
		SimplexPoint(const Vector3& aSupport, const Vector3& bSupport) : aSupport(aSupport), bSupport(bSupport), difference(aSupport - bSupport) {}
	};

	SimplexPoint simplex[4];

	SimplexPoint& a = simplex[0];
	SimplexPoint& b = simplex[1];
	SimplexPoint& c = simplex[2];
	SimplexPoint& d = simplex[3];

	const Vector3& mA = a.difference;
	const Vector3& mB = b.difference;
	const Vector3& mC = c.difference;
	const Vector3& mD = d.difference;

	Vector3 closestPoint;
	Vector3 dir;
	int i = 0;
	for (int iteration = 0; iteration < GJK_MAX_ITERATIONS; ++iteration)
	{
		switch (i)
		{
			case 0:
				//First point, search along A->B
				dir = (shapeB.GetTransform().GetPosition() + tB.GetPosition()) - (shapeA.GetTransform().GetPosition() + tA.GetPosition());

				if (dir.LengthSquared() == 0.f)
					dir = Vector3(1.f, 0.f, 0.f);

				break;
			case 1:
				closestPoint = mA;
				dir *= -1.f;
				break;
			case 2:
			{
				closestPoint = ClosestPointToOriginOnLineSegment(mA, mB);
				
				dir = -closestPoint;
				break;
			}
			case 3:
			{
				closestPoint = ClosestPointToOriginOnTriangle(mA, mB, mC);

				////
				dir = Vector3::Cross(mB-mC, mA-mC);

				//If cross product is not facing origin, flip it
				if (Vector3::Dot(dir, -mC) < 0.f)
					dir *= -1.f;
				break;
			}
			case 4:
			{
				{
					Vector3 da = mA - mD;
					Vector3 db = mB - mD;
					Vector3 dc = mC - mD;
					//I did have a more efficient way of doing this but it is way uglier and has a small issue so it is not used here, see UnusedCollider.txt

					if (Vector3::Dot(mD, GetNormalForFace(da, db, -dc)) > 0.f &&	//Inside DAB
						Vector3::Dot(mD, GetNormalForFace(db, dc, -da)) > 0.f &&	//Inside DBC
						Vector3::Dot(mD, GetNormalForFace(dc, da, -db)) > 0.f &&	//Inside DCA
						Vector3::Dot(mA, GetNormalForFace(mB - mA, mC - mA, da)) > 0.f)//Inside ABC
					{
						//Inside all faces of tetrahedron
						return 0.f;
					}


					//Outside tetrahedron, find each triangle's closest point and use the triangle with the lowest one
					Vector3 triPoints[4] = {
						ClosestPointToOriginOnTriangle(mD, mB, mC),
						ClosestPointToOriginOnTriangle(mD, mC, mA),
						ClosestPointToOriginOnTriangle(mD, mA, mB),
						ClosestPointToOriginOnTriangle(mA, mB, mC)
					};

					int smallestPoint = 0;
					float smallestLengthSq = triPoints[smallestPoint].LengthSquared();
					for (int i = 1; i < 4; ++i)
					{
						float lengthSq = triPoints[i].LengthSquared();
						if (lengthSq < smallestLengthSq)
						{
							smallestPoint = i;
							smallestLengthSq = lengthSq;
						}
					}

					MathematicaDebugState(mA, mB, mC, mD, triPoints[smallestPoint]);

					//Remove point not on closest triangle
					//Note: cases fall through
					switch (smallestPoint)
					{
					case 0: //DBC
						a = b;
					case 1: //DCA
						b = c;
					case 2:	//DAB
						c = d;
						break;
					}

					closestPoint = triPoints[smallestPoint];
					dir = -closestPoint;
					i = 3;
					break;
				}
			}
		}

		//Find new support point
		if (pLineA)
		{
			Vector3 farthestLinePoint = Vector3::Dot(dir, pLineA->end - pLineA->start) > 0.f ? pLineA->end : pLineA->start;

			simplex[i] = SimplexPoint(farthestLinePoint + shapeA.GetFarthestPointInDirection(dir, tA), shapeB.GetFarthestPointInDirection(-dir, tB));
		}
		else
		{
			simplex[i] = SimplexPoint(shapeA.GetFarthestPointInDirection(dir, tA), shapeB.GetFarthestPointInDirection(-dir, tB));
		}

#if GJK_DEBUG
		Debug::PrintLine(CSTR("Current closest point is (", closestPoint, ")"));

		Debug::PrintLine(CSTR(
			iteration, "|point ", i, ", direction (", dir,
			") : a-support(", simplex[i].aSupport, 
			") - b-support(", simplex[i].bSupport, 
			") = minkowski(", simplex[i].difference, ")"));
#endif

		if (i >= 1)
		{
			bool stopIterating = false;

			double dotD = Vector<double, 3>::Dot(dir, simplex[i].difference);
			for (int j = 0; j < i; ++j)
			{
				double dotP = Vector<double, 3>::Dot(dir, simplex[j].difference);

				//Debug::PrintLine(CSTR("dot is ", String::From(dotD-dotP, 0, 100)));

				if (dotD - dotP <= GJK_TOLERANCE)
				{
					stopIterating = true;
					break;
				}
			}

			if (stopIterating) break;
		}

		++i;
	}

	switch (i)
	{
	case 1:
	{
		pointA = a.aSupport;
		pointB = a.bSupport;
		break;
	}
	case 2:
	{
		Vector2 bary = Cartesian2Barycentric(closestPoint, mA, mB);
		pointA = a.aSupport * bary[0] + b.aSupport * bary[1];
		pointB = a.bSupport * bary[0] + b.bSupport * bary[1];
		break;
	}
	case 3:
	case 4:
	{
		Vector3 bary = Cartesian2Barycentric(closestPoint, mA, mB, mC);
		pointA = a.aSupport * bary[0] + b.aSupport * bary[1] + c.aSupport * bary[2];
		pointB = a.bSupport * bary[0] + b.bSupport * bary[1] + c.bSupport * bary[2];
		break;
	}
	}

	float result = closestPoint.Length();

#if GJK_DEBUG
	Debug::PrintLine(CSTR("DONE (", result, ")"));
#endif

	return result;
}

float Collider::MinimumDistanceTo(
	const Transform& transform, 
	const Collider& other, const Transform& otherTransform, 
	Vector3& out_PointA, Vector3& out_PointB, const LineSegment* pLineA) const
{
	float minDist = -1.f;
	Vector3 a, b;

	if (CanCollideWith(other))
		for (size_t i = 0; i < GetShapeCount(); ++i)
			for (size_t j = 0; j < GetShapeCount(); ++j)
			{
				float dist = GJKDist(GetShape(i), transform, other.GetShape(j), otherTransform, a, b, pLineA);

				if (minDist < 0.f || dist < minDist)
				{
					out_PointA = a;
					out_PointB = b;

					if (dist <= 0.f)
						return 0.f;

					
					minDist = dist;
				}
			}

	return minDist;
}
