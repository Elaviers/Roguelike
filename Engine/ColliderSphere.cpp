#include "ColliderSphere.hpp"
#include "ColliderBox.hpp"
#include "GameObject.hpp"
#include "RaycastResult.hpp"

bool ColliderSphere::IntersectsRay(const Ray &ray, RaycastResult &result, const Transform &transform) const
{
	if (!CanCollideWithChannels(ray.channels))
		return false;

	//Assuming ray.direction is normalised

	if (transform.GetScale()[0] != transform.GetScale()[1] || transform.GetScale()[0] != transform.GetScale()[2])
		Debug::PrintLine("WARNING: Sphere collision used with nonuniform scaling");

	float r = _radius * transform.GetScale()[0];

	Vector3 selfToRayOrigin = ray.origin - transform.GetPosition();
	float dot = Vector3::Dot(selfToRayOrigin, ray.direction);
	float distanceSq = selfToRayOrigin.LengthSquared() - r * r;	//The distance from the surface of the sphere to the ray's origin

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
