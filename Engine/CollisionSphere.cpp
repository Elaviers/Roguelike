#include "CollisionSphere.hpp"
#include "Entity.hpp"
#include "RaycastResult.hpp"

bool CollisionSphere::IntersectsRay(const Ray &ray, RaycastResult &result, const Transform & worldTransform) const
{
	//Assuming ray.direction is normalised

	if (worldTransform.GetScale()[0] != worldTransform.GetScale()[1] || worldTransform.GetScale()[0] != worldTransform.GetScale()[2])
		Debug::PrintLine("WARNING: Sphere collision used with nonuniform scaling");

	float r = _radius * worldTransform.GetScale()[0];

	Vector3 selfToRayOrigin = ray.origin - worldTransform.GetPosition();
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

Vector3 CollisionSphere::GetNormalForPoint(const Vector3& point, const Transform& worldTransform) const
{
	return (point - (_transform * worldTransform).GetPosition()).Normalise();
}

Vector3 CollisionSphere::GetPointWithHighestDot(const Vector3& axis, const Transform& worldTransform) const
{
	Transform t = _transform * worldTransform;
	return t.GetPosition() + (axis * _radius * t.GetScale()[0]);
}
