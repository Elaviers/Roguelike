#include "ColliderSphere.h"
#include "ColliderAABB.h"
#include "GameObject.h"
#include "RaycastResult.h"

bool ColliderSphere::IntersectsRay(const Ray &ray, RaycastResult &result, const Transform &transform) const
{
	//Assuming ray.direction is noramlised

	Vector3 selfToRayOrigin = ray.origin - transform.Position();
	float dot = Vector3::Dot(selfToRayOrigin, ray.direction);
	float distanceSq = selfToRayOrigin.LengthSquared() - _radius * _radius;	//The distance from the surface of the sphere to the ray's origin

	if (distanceSq > 0.f && dot > 0.f) //We are not inside the sphere and not facing it
		return false;

	float discriminant = dot * dot - distanceSq;
	if (discriminant < 0.f) //We missed the sphere
		return false;

	result.entryTime = -dot - Maths::SquareRoot(discriminant);
	
	if (result.entryTime < 0.f)
		result.entryTime = 0.f;

	return true;
}
