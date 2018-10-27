#pragma once
#include "Ray.h"
#include "Transform.h"

//Forward declaring these is kinda dumb but oh well
class ColliderAABB;
class ColliderSphere;

class GameObject;

struct RaycastResult;

class Collider
{
protected:
	Collider(const GameObject *parent) : _parent(parent) {}

	const GameObject *_parent;

public:
	virtual ~Collider() {}

	virtual bool IntersectsRay(const Ray&, RaycastResult&) const = 0;

	virtual bool Overlaps(const ColliderAABB&) const = 0;
	virtual bool Overlaps(const ColliderSphere&) const = 0;
};

