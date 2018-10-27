#include "ColliderSphere.h"
#include "ColliderAABB.h"
#include "GameObject.h"
#include "RaycastResult.h"

bool ColliderSphere::IntersectsRay(const Ray &ray, RaycastResult &result) const
{
	//Assuming ray.direction is noramlised

	Vector3 selfToRayOrigin = ray.origin - _parent->transform.Position();			
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

bool ColliderSphere::Overlaps(const ColliderSphere &other) const
{
	float d = (_parent->transform.Position() - other._parent->transform.Position()).LengthSquared();
	float maxD = _radius * _radius + other._radius * other._radius;

	return d <= maxD;
}

bool ColliderSphere::Overlaps(const ColliderAABB &other) const
{
	Vector3 closestPoint = Vector3(
		Maths::Clamp(other.min[0], other.max[0], other._parent->transform.Position()[0]), 
		Maths::Clamp(other.min[1], other.max[1], other._parent->transform.Position()[1]), 
		Maths::Clamp(other.min[2], other.max[2], other._parent->transform.Position()[2]));

	float dist = (_parent->transform.Position() - closestPoint).LengthSquared();
	if (dist <= _radius * _radius)
		return true;

	return false;
}
