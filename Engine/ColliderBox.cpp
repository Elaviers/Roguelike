#include "ColliderBox.hpp"
#include "ColliderSphere.hpp"
#include "Collision.hpp"
#include "GameObject.hpp"
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

bool ColliderBox::IntersectsRay(const Ray &ray, RaycastResult &result, const Transform &worldTransform) const
{
	if (!CanCollideWithChannels(ray.channels))
		return false;

	Transform t = transform * worldTransform;

	Mat4 mat = t.GetRotation().GetQuat().Inverse().ToMatrix();
	//Quaternion invRotation = t.GetRotation().GetQuat().Inverse();

	Vector3 origin = (ray.origin - t.GetPosition()) * mat;
	Vector3 dir = ray.direction * mat;
	
	Vector3 scaledMin = -1.f * extent * t.GetScale();
	Vector3 scaledMax = extent * t.GetScale();

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
