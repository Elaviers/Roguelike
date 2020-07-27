#include "CollisionBox.hpp"
#include "Collision.hpp"
#include "RaycastResult.hpp"
#include <ELCore/Utilities.hpp>
#include <ELMaths/Maths.hpp>
#include <ELMaths/Ray.hpp>

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

	Matrix4 mat = Matrix4::Rotation(t.GetRotation().GetQuat().Inverse());

	Vector3 origin = (Vector4(ray.origin - t.GetPosition(), 1.f) * mat).GetXYZ();
	Vector3 dir = (Vector4(ray.direction, 1.f) * mat).GetXYZ();
	
	Vector3 scaledMin = -1.f * t.GetScale();
	Vector3 scaledMax = t.GetScale();

	float minT, maxT;
	float minT_y, maxT_y;
	FindT(origin.x, dir.x, scaledMin.x, scaledMax.x, minT, maxT);
	FindT(origin.y, dir.y, scaledMin.y, scaledMax.y, minT_y, maxT_y);

	if (minT > maxT_y || minT_y > maxT)
		return false;

	minT = Maths::Max(minT, minT_y);
	maxT = Maths::Min(maxT, maxT_y);

	float minT_z, maxT_z;
	FindT(origin.z, dir.z, scaledMin.z, scaledMax.z, minT_z, maxT_z);

	if (minT > maxT_z || minT_z > maxT)
		return false;

	minT = Maths::Max(minT, minT_z);
	maxT = Maths::Min(maxT, maxT_z);

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
	Vector3 p = (Vector4(point, 1.f) * (_transform * worldTransform).GetInverseTransformationMatrix()).GetXYZ();

	float x = Maths::Abs(point.x), y = Maths::Abs(point.y), z = Maths::Abs(point.z);

	if (x > y)
	{
		if (x > z)
			return Vector3(point.x > 0 ? 1.f : -1.f, 0.f, 0.f);
		else
			return Vector3(0.f, 0.f, point.z > 0 ? 1.f : -1.f);
	}

	if (y > z) return Vector3(0.f, point.y > 0 ? 1.f : -1.f, 0.f);

	return Vector3(0.f, 0.f, point.z > 0 ? 1.f : -1.f);
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

	Matrix4 fullTransform = (_transform * worldTransform).GetTransformationMatrix();
	for (int i = 0; i < 8; ++i)
	{
		points[i] = (Vector4(points[i], 1.f) * fullTransform).GetXYZ();
		
		float dot = points[i].Dot(axis);
		if (dot > maxDot || index < 0)
		{
			index = i;
			maxDot = dot;
		}
	}

	return points[index];
}
