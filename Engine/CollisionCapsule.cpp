#include "CollisionCapsule.hpp"

//Basically just the ray intersection test from CollisionSphere.cpp :/
inline bool SphereRayEntryTime(Vector3 rayStart, const Vector3& rayDir, float sphereRadius, float sphereTranslationY, float &outT)
{
    rayStart[1] -= sphereTranslationY;

    float distanceSq = rayStart.LengthSquared() - sphereRadius * sphereRadius;
    float dot = Vector3::Dot(rayStart, rayDir);

    if (distanceSq > 0.f && dot > 0.f)
        return false;

    float discriminant = dot * dot - distanceSq;
    if (discriminant < 0.f)
        return false;

    outT = -dot - Maths::SquareRoot(discriminant);
    return true;
}


/*
    Excerpt from CollisionSphere.cpp:


                            discriminant
                        -------------------------
                        |						|
    t = -S.D +|- sqrt( (S.D)^2 - (D.D)(S.S - R^2) )
        -----------------------------------------
                            D.D
*/

bool CollisionCapsule::IntersectsRay(const Ray& ray, RaycastResult& result, const Transform& worldTransform) const
{
    Mat4 inv = (_transform * worldTransform).GetInverseTransformationMatrix();
    Vector3 transformedRayOrigin = ray.origin * inv;
    Vector4 transformedRayDirection4 = Vector4(ray.direction, 0.f) * inv;
    Vector3 transformedRayDirection = Vector3(transformedRayDirection4[0], transformedRayDirection4[1], transformedRayDirection4[2]);
    transformedRayDirection.Normalise();

    //The transformed space has the capsule at the origin with its radius and half height
    float dot = Vector3::Dot(transformedRayOrigin, transformedRayDirection);

    if (transformedRayOrigin.LengthSquared() > _halfHeight * _halfHeight && dot > 0.f) 
        return false;   //We are not close enough to the capsule and are not facing it

    float dot_S_D_2D = transformedRayOrigin[0] * transformedRayDirection[0] + transformedRayOrigin[2] * transformedRayDirection[2];
    float dot_S_S_2D = transformedRayOrigin[0] * transformedRayOrigin[0] + transformedRayOrigin[2] * transformedRayOrigin[2];
    float dot_D_D_2D = transformedRayDirection[0] * transformedRayDirection[0] + transformedRayDirection[2] * transformedRayDirection[2];

    //(S.D)^2 - (D.D)(S.S - R^2)
    float cylinderDiscriminant = dot_S_D_2D * dot_S_D_2D - dot_D_D_2D * (dot_S_S_2D - _radius * _radius);
    if (cylinderDiscriminant < 0.f)
        return false;   //Ray does not intersect infinite cylinder

    float cylinderEntry = (-dot_S_D_2D - Maths::SquareRoot(cylinderDiscriminant)) / dot_D_D_2D;
    float cylinderEntryY = transformedRayOrigin[1] + cylinderEntry * transformedRayDirection[1];
    float cylinderHalfHeight = _halfHeight - _radius;

    if (cylinderEntryY > cylinderHalfHeight)
    {
        float t;
        if (!SphereRayEntryTime(transformedRayOrigin, transformedRayDirection, _radius, cylinderHalfHeight, t))
            return false;

        result.entryTime = t;
    }
    else if (cylinderEntryY < -cylinderHalfHeight)
    {
        float t;
        if (!SphereRayEntryTime(transformedRayOrigin, transformedRayDirection, _radius, -cylinderHalfHeight, t))
            return false;

        result.entryTime = t;
    }
    else
        result.entryTime = cylinderEntry;

    if (result.entryTime < 0.f)
        result.entryTime = 0.f;

    return true;
}

Vector3 CollisionCapsule::GetNormalForPoint(const Vector3& point, const Transform& worldTransform) const
{
    return Vector3();
}

inline Vector3 SphereRayExitPoint(float startY, const Vector3& dir,  float radius)
{
    //D.D = 1
    //t = -S.D +|- sqrt( (S.D)^2 - (S.S - R^2) )

    Vector3 rayStart(0.f, startY, 0.f);
    float dot = Vector3::Dot(rayStart, dir);
    float sphereDiscriminant = dot * dot - (rayStart.LengthSquared() - radius * radius);

    Debug::Assert(sphereDiscriminant >= 0.f);

    float sphereExit = -dot + Maths::SquareRoot(sphereDiscriminant);
    return dir * sphereExit;
}

Vector3 CollisionCapsule::GetFarthestPointInDirection(const Vector3& axisIn, const Transform& worldTransform) const
{
    Vector3 axis = axisIn.Normalised() * worldTransform.GetRotation().GetQuat().Inverse().ToMatrix();

    if (axis[1] > 0.f)
        return (Vector3(0.f, _halfHeight - _radius, 0.f) + _radius * axis) * (_transform * worldTransform).GetTransformationMatrix();
    
    return (Vector3(0.f, -_halfHeight + _radius, 0.f) + _radius * axis) * (_transform * worldTransform).GetTransformationMatrix();
}
