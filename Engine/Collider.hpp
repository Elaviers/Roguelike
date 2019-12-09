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
	CollisionChannels _channels;

	Collider(ColliderType type, CollisionChannels channels) : _type(type), _channels(channels) {}

public:
	virtual ~Collider() {}

	const ColliderType& GetType() const { return _type; }
	const CollisionChannels& GetChannels() const { return _channels; }

	void SetChannels(CollisionChannels channels) { _channels = channels; }
	void AddChannels(CollisionChannels channels) { _channels |= channels; }
	void RemoveChannels(CollisionChannels channels) { _channels = (CollisionChannels)(_channels & (~channels)); }
	bool CanCollideWithChannels(CollisionChannels channels) const { return (_channels & channels) != 0; }
	bool CanCollideWith(const Collider& other) const { return CanCollideWithChannels(other._channels); }

	virtual bool IntersectsRay(const Ray&, RaycastResult&, const Transform& _transform) const = 0;

	bool Overlaps(const Collider &other, const Transform &otherTransform, const Transform &_transform) const;
};
