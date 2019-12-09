#pragma once
#include "Collider.hpp"

class ColliderSphere : public Collider
{
	float _radius;

public:
	ColliderSphere(CollisionChannels channels, float radius = 0.f) : Collider(ColliderType::SPHERE, channels), _radius(radius) {}
	virtual ~ColliderSphere() {}

	float GetRadius() const { return _radius; }
	void SetRadius(float radius) { _radius = radius; }

	virtual bool IntersectsRay(const Ray&, RaycastResult&, const Transform& = Transform()) const override;
};
