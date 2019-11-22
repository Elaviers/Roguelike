#pragma once
#include "CollisionChannel.hpp"
#include "Ray.hpp"
#include "Transform.hpp"

enum class ColliderType
{
	CUSTOM,
	BOX,
	SPHERE
};

struct RaycastResult;

class Collider
{
protected:
	const ColliderType _type;
	CollisionChannel _channels;

	Collider(ColliderType type, CollisionChannel channels) : _type(type), _channels(channels) {}

public:
	virtual ~Collider() {}

	const ColliderType& GetType() const { return _type; }
	const CollisionChannel& GetChannels() const { return _channels; }

	void SetChannels(CollisionChannel channels) { _channels = channels; }
	void AddChannels(CollisionChannel channels) { _channels = (CollisionChannel)(_channels | channels); }
	void RemoveChannels(CollisionChannel channels) { _channels = (CollisionChannel)(_channels & (~channels)); }
	bool CanCollideWithChannels(CollisionChannel channels) const { return (_channels & channels) != 0; }
	bool CanCollideWith(const Collider& other) const { return CanCollideWithChannels(other._channels); }

	virtual bool IntersectsRay(const Ray&, RaycastResult&, const Transform& _transform) const = 0;

	bool Overlaps(const Collider &other, const Transform &otherTransform, const Transform &_transform) const;
};
