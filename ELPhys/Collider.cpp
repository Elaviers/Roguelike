#include "Collider.hpp"
#include "Collision.hpp"
#include "CollisionBox.hpp"
#include "CollisionSphere.hpp"
#include <ELCore/List.hpp>
#include <ELCore/Pool.hpp>
#include <ELMaths/LineSegment.hpp>
#include <ELMaths/Ray.hpp>
#include <ELMaths/Transform.hpp>
#include <ELMaths/Vector2.hpp>

bool Collider::IntersectsRay(const Transform& transform, const Ray& ray, ECollisionChannels channels, RaycastResult& result) const
{
	if (CanCollideWithChannels(channels))
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
	return c.Dot(a) * b - c.Dot(b) * a;
}

constexpr const int GJK_MAX_ITERATIONS = 50;

constexpr const int EPA_MAX_ITERATIONS = 250;

//GJK will terminate if the dot product between a point and an edge normal is lower than this
constexpr const float GJK_TOUCH_TOLERANCE = 0.001f;

//GJKDist will terminate if a the dot product between an existing point and the direction and the dot product between the new point and the direction is lower than this
constexpr const double GJK_TOLERANCE = 1e-10;

constexpr const double EPA_TOLERANCE = 1e-10;

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

inline Vector3 ClosestPointToOriginOnPlane(const Vector3& planePoint, const Vector3& planeNormal)
{
	//Debug::Assert(Maths::AlmostEquals(planeNormal.LengthSquared(), 1.f, .001f), "ClosestPointToOriginOnPlane requires a normalised normal vector...");

	return planePoint.Dot(planeNormal) * planeNormal;
}

inline Vector3 ClosestPointToOriginOnLineSegment(const Vector3& a, const Vector3& ab)
{
	float length = ab.Length();

	if (length == 0.f)
		return a;

	Vector3 d = ab / length;
	float dot = (-a).Dot(d);

	if (dot <= 0.f) return a;
	if (dot >= length) return a + ab;
	return a + d * dot;
}

Vector3 ClosestPointToOriginOnTriangle(const Vector3& a, const Vector3& b, const Vector3& c)
{
	if (a == b) return ClosestPointToOriginOnLineSegment(a, c - a);
	if (a == c || b == c) return ClosestPointToOriginOnLineSegment(a, b - a);

	Vector3 ab = b - a;
	Vector3 bc = c - b;
	Vector3 ca = a - c;

	//A.-N[AB]
	if ((a).Dot(TripleCross(ab, ca, ab)) <= 0.f)
	{
		if ((a).Dot(-ab) <= 0.f)				//A.BA (outside a on ab)
		{
			if ((a).Dot(ca) <= 0.f) return a;	//A.CA (outside a on ca)
			return ClosestPointToOriginOnLineSegment(c, ca);
		}

		if ((b).Dot(ab) <= 0.f)					//B.AB (outside b on ab)
		{
			if ((b).Dot(-bc) <= 0.f) return b;	//B.CB (outside b on bc)
			return ClosestPointToOriginOnLineSegment(b, bc);
		}

		return ClosestPointToOriginOnLineSegment(a, ab);
	}

	//B.-N[BC]
	if ((b).Dot(TripleCross(bc, ab, bc)) <= 0.f)
	{
		if ((b).Dot(-bc) <= 0.f) return b;		//B.CB (outside b on bc)

		if ((c).Dot(bc) <= 0.f)					//B.BC (outside c on bc)
		{
			if ((c).Dot(-ca) <= 0.f) return c;	//C.AC (outside c on ca)

			return ClosestPointToOriginOnLineSegment(c, ca);
		}

		return ClosestPointToOriginOnLineSegment(b, bc);
	}

	//C.-N[CA]
	if ((c).Dot(TripleCross(ca, -ab, ca)) <= 0.f)
	{
		if ((a).Dot(ca) <= 0.f) return a;		//A.CA (outside a on ca)
		if ((c).Dot(-ca) <= 0.f) return c;		//C.AC (outside c on ca)
		return ClosestPointToOriginOnLineSegment(c, ca);
	}

	return ClosestPointToOriginOnPlane(a, (ab).Cross(bc).Normalised());
}

//Returns vector perpendicular to the two sides in the direction of dir
inline Vector3 GetNormalForFace(const Vector3& side1, const Vector3& side2, const Vector3& dir)
{
	Vector3 result = (side1).Cross(side2);

	if ((result).Dot(dir) < 0.f)
		return -result;

	return result;
}

struct Face
{
	Vector3 a, b, c;
	Vector3 closestPointToOrigin;
	float closestPointLengthSq;

	Vector3 normal;

	Face() : closestPointLengthSq(0.f) {}
	Face(const Vector3& a, const Vector3& b, const Vector3& c) :
		a(a), b(b), c(c)
	{
		closestPointToOrigin = ClosestPointToOriginOnTriangle(a, b, c);
		closestPointLengthSq = closestPointToOrigin.LengthSquared();
		normal = GetNormalForFace(b - a, c - a, a);
	}
};

void AddEdge(List<Pair<Vector3>>& edges, const Vector3& a, const Vector3& b)
{
	for (auto it = edges.begin(); it.IsValid(); ++it)
	{
		if (a == it->second && b == it->first)
		{
			edges.Remove(it);
			return;
		}
	}

	edges.Add(Pair<Vector3>(a, b));
}

void InsertFace(List<Face>& closestFaces, const Face& face)
{
	for (auto it = closestFaces.begin(); it.IsValid(); ++it)
		if (it->closestPointLengthSq > face.closestPointLengthSq)
		{
			closestFaces.Insert(face, it);
			return;
		}

	closestFaces.Add(face);
}

inline Vector3 Support(
	const CollisionShape& shapeA, const Transform& tA,
	const CollisionShape& shapeB, const Transform& tB,
	const LineSegment* pLineA,
	const Vector3& dir)
{
	if (pLineA)
	{
		Vector3 farthestLinePoint = (dir).Dot(pLineA->end - pLineA->start) > 0.f ? pLineA->end : pLineA->start;
		return (farthestLinePoint + shapeA.GetFarthestPointInDirection(dir, tA)) - shapeB.GetFarthestPointInDirection(-dir, tB);
	}
	
	return shapeA.GetFarthestPointInDirection(dir, tA) - shapeB.GetFarthestPointInDirection(-dir, tB);
}

Vector3 EPA(
	const Vector3 simplex[4],
	const CollisionShape& shapeA, const Transform& tA,
	const CollisionShape& shapeB, const Transform& tB,
	const LineSegment* pLineA = nullptr)
{
	typedef MultiPool<byte, 1000> PoolType;
	static PoolType pool;

	pool.Clear();

	//Array of closest faces, closest first, farthest last
	List<Face> closestFaces(NewHandler(&pool, &PoolType::NewArray), DeleteHandler(&pool, &PoolType::DeleteHandler));
	
	closestFaces.Add(Face(simplex[3], simplex[0], simplex[1]));
	InsertFace(closestFaces, Face(simplex[3], simplex[1], simplex[2]));
	InsertFace(closestFaces, Face(simplex[3], simplex[2], simplex[0]));
	InsertFace(closestFaces, Face(simplex[0], simplex[1], simplex[2]));

	for (int i = 0; i < EPA_MAX_ITERATIONS; ++i)
	{
		Face& closestFace = *closestFaces.begin();
		Vector3 dir = closestFace.normal;

		Vector3 newPoint = Support(shapeA, tA, shapeB, tB, pLineA, dir);

		double newDot = ((Vector3T<double>)dir).Dot(newPoint);
		double oldDot = ((Vector3T<double>)dir).Dot(closestFace.a);
		if (newDot - oldDot <= EPA_TOLERANCE)
		{
			return closestFace.closestPointToOrigin;
		}

		List<Pair<Vector3>> edges(NewHandler(&pool, &PoolType::NewArray), DeleteHandler(&pool, &PoolType::DeleteHandler));

		//Remove any faces that the new point is in front of
		for (auto it = closestFaces.begin(); it.IsValid();)
		{
			//Point on iterator face -> new point . iterator normal
			if ((newPoint - it->a).Dot(it->normal) > 0.f)
			{
				AddEdge(edges, it->a, it->b);
				AddEdge(edges, it->b, it->c);
				AddEdge(edges, it->c, it->a);

				it = closestFaces.Remove(it);
			}
			else
				++it;
		}

		for (const Pair<Vector3>& edge : edges)
			InsertFace(closestFaces, Face(newPoint, edge.first, edge.second));
	}

	return closestFaces.begin()->closestPointToOrigin;
}

EOverlapResult GJK(const CollisionShape& shapeA, const Transform& tA, const CollisionShape& shapeB, const Transform& tB, const LineSegment* pLineA, Vector3* out_PenetrationVector)
{
	//The simplex is a tetrahedron inside the minkowski difference
	Vector3 simplex[4];
	Vector3& a = simplex[0], & b = simplex[1], & c = simplex[2], & d = simplex[3];

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

			Vector3 ba = a - b;

			//(BA X B0) X BA
			dir = TripleCross(ba, -b, ba);
			break;
		}
		case 3:
		{
			//The simplex is a triangle that contains the origin in its respective space
			//Look for the fourth support along the normal which is facing the origin

			dir = (b - c).Cross(a - c);

			//If cross product is not facing origin, flip it
			if ((dir).Dot(-c) < 0.f)
				dir *= -1.f;

			break;
		}
		case 4:
		{
			//The simplex is a tetrahedron whose base (points 0, 1, 2) is on the opposite side of the origin from its peak (point 3)
			//Check if the tetrahedron contains the origin by checking the dot products of the normals of the 3 faces related to the peak
			//The base's normal doesn't have to be checked as it is on the opposite side of the origin from the peak

			//Edges
			Vector3 dc = c - d;
			Vector3 db = b - d;

			Vector3 bcd = (dc).Cross(db);
			float dot = (bcd).Dot(d);

			if (Maths::AlmostEquals(dot, 0.f, GJK_TOUCH_TOLERANCE))
				return EOverlapResult::TOUCHING;

			if (dot <= 0.f)
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

			Vector3 abd = (db).Cross(da);
			dot = (abd).Dot(d);

			if (Maths::AlmostEquals(dot, 0.f, GJK_TOUCH_TOLERANCE))
				return EOverlapResult::TOUCHING;

			if (dot <= 0.f)
			{
				//Remove point C (2)
				simplex[2] = simplex[3];
				i = 3;

				dir = abd;
				break;
			}

			Vector3 acd = (da).Cross(dc);
			dot = (acd).Dot(d);

			if (Maths::AlmostEquals(dot, 0.f, GJK_TOUCH_TOLERANCE))
				return EOverlapResult::TOUCHING;

			if (dot <= 0.f)
			{
				//Remove point B (1)
				simplex[1] = simplex[2];
				simplex[2] = simplex[3];
				i = 3;

				dir = acd;
				break;
			}

			if (out_PenetrationVector)
				*out_PenetrationVector = EPA(simplex, shapeA, tA, shapeB, tB, pLineA);

			return EOverlapResult::OVERLAPPING;
		}
		}

		//Find new support point
		simplex[i] = Support(shapeA, tA, shapeB, tB, pLineA, dir);

		//Fail if the new point did not go past the origin
		if ((simplex[i]).Dot(dir) < 0.f)
			return EOverlapResult::SEPERATE;

		++i;
	}

	//Failsafe
	return EOverlapResult::OVERLAPPING;
}

EOverlapResult Collider::Overlaps(const Transform& transform, const Collider& other, const Transform& otherTransform, const LineSegment* lineA, Vector3* out_PenetrationVector) const
{
	bool isTouching = false;

	if (CanCollideWith(other))
		for (size_t i = 0; i < GetShapeCount(); ++i)
			for (size_t j = 0; j < other.GetShapeCount(); ++j)
			{
				const CollisionShape& shape = GetShape(i);
				const CollisionShape& otherShape = other.GetShape(j);

				float distanceSq = ((shape.GetTransform().GetPosition() + transform.GetPosition()) - (otherShape.GetTransform().GetPosition() + otherTransform.GetPosition())).LengthSquared();
				float shapeRadius = shape.GetMaximumScaledRadius() * Maths::Min(transform.GetScale().GetData(), 3);
				float otherShapeRadius = otherShape.GetMaximumScaledRadius() * Maths::Min(otherTransform.GetScale().GetData(), 3);
				float combinedRadii = shapeRadius + otherShapeRadius;
				if (true)
				{
					EOverlapResult result = GJK(shape, transform, otherShape, otherTransform, lineA, out_PenetrationVector);
					if (result == EOverlapResult::OVERLAPPING)
						return EOverlapResult::OVERLAPPING;

					if (!isTouching && result == EOverlapResult::TOUCHING)
						isTouching = true;
				}
			}

	return isTouching ? EOverlapResult::TOUCHING : EOverlapResult::SEPERATE;
}

inline Vector2 Cartesian2Barycentric(const Vector3& p, const Vector3& a, const Vector3& b)
{
	Vector2 result;
	result.x = (p - b).Length() / (a - b).Length();
	result.y = 1.f - result.x;
	return result;
}

Vector3 Cartesian2Barycentric(const Vector3& p, const Vector3& a, const Vector3& b, const Vector3& c)
{
	if (a == b)
	{
		Vector2 bary2D = Cartesian2Barycentric(p, a, c);
		return Vector3(bary2D.x, 0.f, bary2D.y);
	}

	if (a == c || b == c)
	{
		Vector2 bary2D = Cartesian2Barycentric(p, a, b);
		return Vector3(0.f, bary2D.x, bary2D.y);
	}

	Vector3 result;
	Vector3 ab = b - a, ac = c - a, ap = p - a;
	float ab_ab = (ab).Dot(ab);
	float ab_ac = (ab).Dot(ac);
	float ac_ac = (ac).Dot(ac);
	float ap_ab = (ap).Dot(ab);
	float ap_ac = (ap).Dot(ac);
	float denom = ab_ab * ac_ac - ab_ac * ab_ac;

	if (denom == 0.f)
	{
		Vector2 bary2D = Cartesian2Barycentric(p, a, b);
		return Vector3(bary2D.x, bary2D.y, 0.f);
	}

	result.y = (ac_ac * ap_ab - ab_ac * ap_ac) / denom;
	result.z = (ab_ab * ap_ac - ab_ac * ap_ab) / denom;
	result.x = 1.0f - result.y - result.z;
	return result;
}

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
			dir = (mB - mC).Cross(mA - mC);

			//If cross product is not facing origin, flip it
			if ((dir).Dot(-mC) < 0.f)
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

				if ((mD).Dot(GetNormalForFace(da, db, -dc)) > 0.f &&	//Inside DAB
					(mD).Dot(GetNormalForFace(db, dc, -da)) > 0.f &&	//Inside DBC
					(mD).Dot(GetNormalForFace(dc, da, -db)) > 0.f &&	//Inside DCA
					(mA).Dot(GetNormalForFace(mB - mA, mC - mA, da)) > 0.f)//Inside ABC
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
			Vector3 farthestLinePoint = (dir).Dot(pLineA->end - pLineA->start) > 0.f ? pLineA->end : pLineA->start;

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

			double dotD = ((Vector3T<double>)dir).Dot(simplex[i].difference);
			for (int j = 0; j < i; ++j)
			{
				double dotP = ((Vector3T<double>)dir).Dot(simplex[j].difference);

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
		pointA = a.aSupport * bary.x + b.aSupport * bary.y;
		pointB = a.bSupport * bary.x + b.bSupport * bary.y;
		break;
	}
	case 3:
	case 4:
	{
		Vector3 bary = Cartesian2Barycentric(closestPoint, mA, mB, mC);
		pointA = a.aSupport * bary.x + b.aSupport * bary.y + c.aSupport * bary.z;
		pointB = a.bSupport * bary.x + b.bSupport * bary.y + c.bSupport * bary.z;
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

inline Vector3 ScaleRelativeToPoint(const Vector3& x, const Vector3& point, float scale)
{
	return Vector3(
		((x.x - point.x) * scale) + point.x,
		((x.y - point.y) * scale) + point.y,
		((x.z - point.z) * scale) + point.z
	);
}

Pair<Vector3> Collider::GetShallowContactPoints(
	const Transform& transform, float shrink,
	const Collider& other, const Transform& otherTransform, float otherShrink) const
{
	//TODO: make this work properly!!!!!!!

	Transform tA(transform.GetPosition(), transform.GetRotation(), transform.GetScale() * shrink);
	Transform tB(otherTransform.GetPosition(), otherTransform.GetRotation(), otherTransform.GetScale() * otherShrink);

	Vector3 a, b;
	if (MinimumDistanceTo(tA, other, tB, a, b))
	{
		return Pair<Vector3>(ScaleRelativeToPoint(a, tA.GetPosition(), 1.f / shrink), ScaleRelativeToPoint(b, tB.GetPosition(), 1.f / otherShrink));
	}

	return Pair<Vector3>();
}
