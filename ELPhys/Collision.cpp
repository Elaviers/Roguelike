#include "Collision.hpp"
#include <ELMaths/Ray.hpp>
#include <ELMaths/Transform.hpp>

namespace Collision
{
	bool SphereOverlapsSphere(
		const Transform& t1, float r1,
		const Transform& t2, float r2)
	{
		float d = (t1.GetPosition() - t2.GetPosition()).LengthSquared();
		float maxD = r1 * r1 + r2 * r2;

		return d <= maxD;
	}

	bool SphereOverlapsAABB(
		const Transform& t1, float r1,
		const Vector3& min2, const Vector3& max2)
	{
		Vector3 closestPoint = Vector3(
			Maths::Clamp(t1.GetPosition().x, min2.x, max2.x),
			Maths::Clamp(t1.GetPosition().y, min2.y, max2.y),
			Maths::Clamp(t1.GetPosition().z, min2.z, max2.z));

		float dist = (t1.GetPosition() - closestPoint).LengthSquared();
		if (dist <= r1 * r1)
			return true;

		return false;
	}

	bool SphereOverlapsBox(
		const Transform& t1, float r1,
		const Transform& t2)
	{
		Transform invRotationTransform = Transform(Vector3(), t2.GetRotation().Inverse());
		Transform tt2 = t2 * invRotationTransform;

		//todo: verify
		return SphereOverlapsAABB(t1 * invRotationTransform, r1, tt2.GetPosition() - tt2.GetScale(), tt2.GetPosition() + tt2.GetScale());
	}

	bool BoxOverlapsBox(const Transform& t1, const Transform& t2)
	{
		Vector3 e1 = t1.GetScale();
		Vector3 e2 = t2.GetScale();

		const Quaternion& r1 = t1.GetRotation().GetQuat();
		const Quaternion& r2 = t2.GetRotation().GetQuat();

		Vector3 axes[15] =
		{	//FACE AXES
			r1.GetForwardVector(), r1.GetUpVector(), r1.GetRightVector(),
			r2.GetForwardVector(), r2.GetUpVector(), r2.GetRightVector()
		};

		const Vector3& fv1 = axes[0], & uv1 = axes[1], & rv1 = axes[2], & fv2 = axes[3], & uv2 = axes[4], & rv2 = axes[5];

		//EDGE AXES
		for (int i = 0; i < 3; ++i)
			for (int j = 0; j < 3; ++j)
				axes[i * 3 + j + 6] = axes[i].Cross(axes[3 + j]);

		for (int i = 0; i < 15; ++i)
		{
			float pDelta = Maths::Abs(axes[i].Dot(t2.GetPosition() - t1.GetPosition()));

			float total =
				Maths::Abs(axes[i].Dot(rv1 * e1.x)) +
				Maths::Abs(axes[i].Dot(uv1 * e1.y)) +
				Maths::Abs(axes[i].Dot(fv1 * e1.z)) +
				Maths::Abs(axes[i].Dot(rv2 * e2.x)) +
				Maths::Abs(axes[i].Dot(uv2 * e2.y)) +
				Maths::Abs(axes[i].Dot(fv2 * e2.z));

			if (pDelta > total)
				return false;
		}

		return true;
	}

	float IntersectRayPlane(const Ray& r, const Vector3& planePoint, const Vector3& planeNormal)
	{
		float denominator = r.direction.Dot(planeNormal);
		if (denominator == 0.f) return 0.f;	//Axes are perpendicular

		float t = (planePoint - r.origin).Dot(planeNormal) / denominator;
		return t;
	}

	float IntersectRayDisc(const Ray& r, const Vector3& centre, const Vector3& normal, float radius)
	{
		float t = IntersectRayPlane(r, centre, normal);
		if (t)
		{
			Vector3 delta = r.origin + r.direction * t - centre;
			if (delta.LengthSquared() > radius * radius)
				return 0.f;	//Too far
		}

		return t;
	}

	float IntersectRayRing(const Ray& r, const Vector3& centre, const Vector3& normal, float inner, float outer)
	{
		float t = IntersectRayPlane(r, centre, normal);
		if (t)
		{
			Vector3 delta = r.origin + r.direction * t - centre;
			float l = delta.LengthSquared();
			if (l < inner * inner || l > outer * outer)
				return 0.f;	//Not in ring
		}

		return t;
	}

	float IntersectRayRect(const Ray& r, const Vector3& centre, const Vector3& normal, const Vector3& w, const Vector3& h)
	{
		float t = IntersectRayPlane(r, centre, normal);
		if (t)
		{
			Vector3 delta = r.origin + r.direction * t - centre;

			float halfWidthSq = w.LengthSquared();
			float halfHeightSq = h.LengthSquared();

			float rightExtent = (w * w).Dot(delta) / halfWidthSq;
			float upExtent = (h * h).Dot(delta) / halfHeightSq;

			if (rightExtent * rightExtent > halfWidthSq || upExtent * upExtent > halfHeightSq)
				return 0.f;	//Outside
		}

		return t;
	}
}
