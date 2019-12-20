#include "CollisionBox.hpp"
#include "Collision.hpp"
#include "Entity.hpp"
#include "Maths.hpp"
#include "RaycastResult.hpp"
#include "Utilities.hpp"

#include <float.h>

inline void FindT(const float &originComponent, const float &directionComponent, const float &minComponent, const float &maxComponent, float &minT, float &maxT)
{
	if (directionComponent == 0.f)
	{
		if (originComponent == minComponent)
			minT = 0.f;
		else if (originComponent > minComponent)
			minT = FLT_MAX;
		else
			minT = -FLT_MAX;

		if (originComponent == maxComponent)
			maxT = 0.f;
		else if (originComponent > maxComponent)
			maxT = FLT_MAX;
		else
			maxT = -FLT_MAX;
	}
	else if (directionComponent > 0.f)
	{
		minT = (minComponent - originComponent) / directionComponent;
		maxT = (maxComponent - originComponent) / directionComponent;
	}
	else
	{
		minT = (maxComponent - originComponent) / directionComponent;
		maxT = (minComponent - originComponent) / directionComponent;
	}
}

bool CollisionBox::IntersectsRay(const Ray &ray, RaycastResult &result, const Transform &worldTransform) const
{
	Transform t = _transform * worldTransform;

	Mat4 mat = t.GetRotation().GetQuat().Inverse().ToMatrix();

	Vector3 origin = (ray.origin - t.GetPosition()) * mat;
	Vector3 dir = ray.direction * mat;
	
	Vector3 scaledMin = -1.f * t.GetScale();
	Vector3 scaledMax = t.GetScale();

	float minT, maxT;
	float minT_y, maxT_y;
	FindT(origin[0], dir[0], scaledMin[0], scaledMax[0], minT, maxT);
	FindT(origin[1], dir[1], scaledMin[1], scaledMax[1], minT_y, maxT_y);

	if (minT > maxT_y || minT_y > maxT)
		return false;

	minT = Utilities::Max(minT, minT_y);
	maxT = Utilities::Min(maxT, maxT_y);

	float minT_z, maxT_z;
	FindT(origin[2], dir[2], scaledMin[2], scaledMax[2], minT_z, maxT_z);

	if (minT > maxT_z || minT_z > maxT)
		return false;

	minT = Utilities::Max(minT, minT_z);
	maxT = Utilities::Min(maxT, maxT_z);

	if (maxT < 0.f) return false; //Should probably check if behind a bit earlier
	
	result.entryTime = minT < 0.f ? 0.f : minT;
	return true;
}

/*
	R = O + dt

	(d.x)(t) + O.x = min.x
	d.x(t) = min.x - O.x
	t = (min.x - O.x)/d.x
*/

Vector3 CollisionBox::GetNormalForPoint(const Vector3& point, const Transform& worldTransform) const
{
	Vector3 p = point * (_transform * worldTransform).GetInverseTransformationMatrix();

	float x = Maths::Abs(point[0]), y = Maths::Abs(point[1]), z = Maths::Abs(point[2]);

	if (x > y)
	{
		if (x > z)
			return Vector3(point[0] > 0 ? 1.f : -1.f, 0.f, 0.f);
		else
			return Vector3(0.f, 0.f, point[2] > 0 ? 1.f : -1.f);
	}

	if (y > z) return Vector3(0.f, point[1] > 0 ? 1.f : -1.f, 0.f);

	return Vector3(0.f, 0.f, point[2] > 0 ? 1.f : -1.f);
}

Vector3 CollisionBox::GetFarthestPointInDirection(const Vector3& axis, const Transform& worldTransform) const
{
	Vector3 points[8] = {
		Vector3(-1, -1, -1),
		Vector3(-1, -1, 1),
		Vector3(-1, 1, -1),
		Vector3(-1, 1, 1),
		Vector3(1, -1, -1),
		Vector3(1, -1, 1),
		Vector3(1, 1, -1),
		Vector3(1, 1, 1)
	};

	int index = -1;
	float maxDot = 0.f;

	Mat4 fullTransform = (_transform * worldTransform).GetTransformationMatrix();
	for (int i = 0; i < 8; ++i)
	{
		points[i] = points[i] * fullTransform;
		
		float dot = Vector3::Dot(points[i], axis);
		if (dot > maxDot || index < 0)
		{
			index = i;
			maxDot = dot;
		}
	}

	return points[index];
}
