#include "ColliderAABB.h"
#include "ColliderSphere.h"
#include "GameObject.h"
#include "Maths.h"
#include "RaycastResult.h"
#include "Utilities.h"

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

bool ColliderAABB::IntersectsRay(const Ray &ray, RaycastResult &result) const
{
	float minT, maxT;
	float minT_y, maxT_y;
	FindT(ray.origin[0], ray.direction[0], min[0], max[0], minT, maxT);
	FindT(ray.origin[1], ray.direction[1], min[1], max[1], minT_y, maxT_y);

	if (minT > maxT_y || minT_y > maxT)
		return false;

	minT = Utilities::Max(minT, minT_y);
	maxT = Utilities::Min(maxT, maxT_y);

	float minT_z, maxT_z;
	FindT(ray.origin[2], ray.direction[2], min[2], max[2], minT_z, maxT_z);

	if (minT > maxT_z || minT_z > maxT)
		return false;

	minT = Utilities::Max(minT, minT_z);
	maxT = Utilities::Min(maxT, maxT_z);

	if (maxT < 0.f) return false; //Should probably check if behind a bit earlier

	result.entryTime = minT < 0.f ? 0.f : minT;
	//result.exitTime = maxT;
	return true;
}

bool ColliderAABB::Overlaps(const ColliderAABB &other) const
{
	if (other.max[0] < min[0] || other.max[1] < min[1] || other.max[2] < min[2] || other.min[0] > max[0] || other.min[1] > max[1] || other.min[2] > max[2])
		return false;

	return true;
}


/*
	R = O + dt

	(d.x)(t) + O.x = min.x
	d.x(t) = min.x - O.x
	t = (min.x - O.x)/d.x
*/
