#include "CollisionSphere.hpp"
#include "RaycastResult.hpp"
#include <ELMaths/Ray.hpp>

bool CollisionSphere::IntersectsRay(const Ray &ray, RaycastResult &result, const Transform & worldTransform) const
{
	/*
		SPHERE:		||P||^2 = r^2
		RAY:		RAY(t) = S + Dt
		
		AT THE OVERLAP:
		||RAY(t)||^2 = ||P||^2 = r^2
		||RAY(t)||^2 = r^2
		||S + Dt||^2 = r^2
		(S+Dt)(S+Dt) = r^2
		S.S + 2 * S.D * t + (Dt)^2 = r^2
		(Dt)^2   +  2 * S.D * t + S.S = r^2
		D.D * t^2 + 2 * S.D * t + S.S = r^2

		Rearrange to polynomial:
		D.D * t^2 + 2 * S.D * t + S.S - r^2 = 0

		Apply quadratic formula...
		a = D.D
		b = 2 * 2.D
		c = S.S - r^2

		t = -2 * S.D +|- sqrt((2 * S.D)^2 - 4(D.D)(S.S - R^2))
			--------------------------------------------------
								2 * D.D

___________________________________________________________________
								discriminant
							--------------------------
							|						|
		t = -S.D +|- sqrt( (S.D)^2 - (D.D)(S.S - R^2) )
			-----------------------------------------
								D.D
___________________________________________________________________

		If D*D = 1:
		t = -S.D +|- sqrt( (S.D)^2 - (S.S - R^2) )
	*/


	Matrix4 inv = (_transform * worldTransform).GetInverseTransformationMatrix();
	Vector3 transformedRayOrigin = (Vector4(ray.origin, 1.f) * inv).GetXYZ();
	Vector4 transformedRayDirection4 = Vector4(ray.direction, 0.f) * inv;
	Vector3 transformedRayDirection = Vector3(transformedRayDirection4[0], transformedRayDirection4[1], transformedRayDirection4[2]);
	transformedRayDirection.Normalise();

	//The transformed space has the sphere at the origin with its radius
	float distanceSq = transformedRayOrigin.LengthSquared() - _radius * _radius;	//The distance from the surface of the sphere to the ray's origin
	
	//Equivalent to transformedRayOrigin.LengthSquared() * cos(theta)
	float dot = transformedRayOrigin.Dot(transformedRayDirection);

	if (distanceSq > 0.f && dot > 0.f)
		return false;	//Ray does not start inside or does not face sphere
	
	float discriminant = dot * dot - distanceSq;
	if (discriminant < 0.f) 
		return false;	//Ray doesn't intersect sphere

	result.entryTime = -dot - Maths::SquareRoot(discriminant);
	if (result.entryTime < 0.f)
		result.entryTime = 0.f;

	return true;
}

Vector3 CollisionSphere::GetNormalForPoint(const Vector3& point, const Transform& worldTransform) const
{
	return (point - (_transform * worldTransform).GetPosition()).Normalise();
}

Vector3 CollisionSphere::GetFarthestPointInDirection(const Vector3& axis, const Transform& worldTransform) const
{
	return worldTransform.GetPosition() + _radius * axis.Normalised() * worldTransform.GetScale();
}
