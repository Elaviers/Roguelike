#include "Frustum.hpp"

Vector3 ClosestPointOnPlane(const Vector3& planePoint, const Vector3& planeNormal, const Vector3& p)
{
	return (planePoint - p).Dot(planeNormal) * planeNormal + p;
}

bool Frustum::OverlapsSphere(const Vector3& centre, float radius) const
{
	//todo- This could surely be faster?!

	Vector3 position2centre = centre - position;

	if ((centre - farBtmLeft).Dot(normalFar) > 0.f && (centre - ClosestPointOnPlane(farBtmLeft, normalFar, centre)).LengthSquared() >= (radius * radius))
		return false;

	if (position2centre.Dot(normalRight) > 0.f && (centre - ClosestPointOnPlane(position, normalRight, centre)).LengthSquared() >= (radius * radius))
		return false;

	if (position2centre.Dot(normalLeft) > 0.f && (centre - ClosestPointOnPlane(position, normalLeft, centre)).LengthSquared() >= (radius * radius))
		return false;

	if (position2centre.Dot(normalTop) > 0.f && (centre - ClosestPointOnPlane(position, normalTop, centre)).LengthSquared() >= (radius * radius))
		return false;

	if (position2centre.Dot(normalBottom) > 0.f && (centre - ClosestPointOnPlane(position, normalBottom, centre)).LengthSquared() >= (radius * radius))
		return false;

	return true;
}

bool Frustum::OverlapsAABB(const Vector3& min, const Vector3& max) const
{
	Vector3 aabb[8] = { 
		min, 
		Vector3(min.x, min.y, max.z), 
		Vector3(min.x, max.y, min.z), 
		Vector3(min.x, max.y, max.z),
		Vector3(max.x, min.y, min.z), 
		Vector3(max.x, min.y, max.z), 
		Vector3(max.x, max.y, min.z), 
		max 
	};

	
	for (int i = 0; i < 6; ++i)
	{
		const Vector3& n = normals[i];
		const Vector3& p = points[i];

		int outside = 0;
		for (int j = 0; j < 8; ++j)
			if ((aabb[j] - p).Dot(n) > 0.f) ++outside;

		if (outside == 8) return false;
	}
	
	int outside;
	outside = 0; for (int i = 0; i < 8; ++i) if (points[i].x > max.x) ++outside; if (outside == 8) return false;
	outside = 0; for (int i = 0; i < 8; ++i) if (points[i].x < min.x) ++outside; if (outside == 8) return false;
	outside = 0; for (int i = 0; i < 8; ++i) if (points[i].y > max.y) ++outside; if (outside == 8) return false;
	outside = 0; for (int i = 0; i < 8; ++i) if (points[i].y < min.y) ++outside; if (outside == 8) return false;
	outside = 0; for (int i = 0; i < 8; ++i) if (points[i].z > max.z) ++outside; if (outside == 8) return false;

	return true;
}
