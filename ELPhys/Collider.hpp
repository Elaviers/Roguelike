#pragma once
#include "CollisionChannels.hpp"
#include "CollisionShape.hpp"
#include <ELCore/Buffer.hpp>
#include <ELCore/Pair.hpp>
#include <ELMaths/Vector3.hpp>

struct LineSegment;
struct Ray;
struct RaycastResult;

class Transform;

enum class EOverlapResult
{
	SEPERATE = 0,
	OVERLAPPING = 1,
	TOUCHING = 2
};

class Collider
{
	Buffer<CollisionShape*> _shapes;

	ECollisionChannels _channels;

public:
	Collider() : _channels(ECollisionChannels::NONE) {}

	Collider(ECollisionChannels channels, const CollisionShape& shape) : _channels(channels), _shapes(shape.Clone()) {}

	Collider(ECollisionChannels channels, const Buffer<CollisionShape>& shapes = Buffer<CollisionShape>()) : _channels(channels)
	{
		_shapes.SetSize(shapes.GetSize());

		for (size_t i = 0; i < _shapes.GetSize(); ++i)
			_shapes[i] = shapes[i].Clone();
	}

	Collider(const Collider& other) : _channels(other._channels)
	{
		_shapes.SetSize(other.GetShapeCount());

		for (size_t i = 0; i < _shapes.GetSize(); ++i)
			_shapes[i] = other.GetShape(i).Clone();
	}

	Collider(Collider&& other) noexcept : _channels(other._channels), _shapes(other._shapes)
	{
		other._shapes.Clear();
	}

	Collider& operator=(const Collider& other)
	{
		_channels = other._channels;
		_shapes.SetSize(other.GetShapeCount());

		for (size_t i = 0; i < _shapes.GetSize(); ++i)
			_shapes[i] = other.GetShape(i).Clone();

		return *this;
	}

	Collider& operator=(Collider&& other) noexcept
	{
		_channels = other._channels;
		_shapes = other._shapes;

		other._shapes.Clear();

		return *this;
	}

	~Collider()
	{
		for (size_t i = 0; i < _shapes.GetSize(); ++i)
			delete _shapes[i];
	}

	const ECollisionChannels& GetChannels() const { return _channels; }

	template <typename T>
	T& AddShape(const T& shape)										{ return (T&)*_shapes.Add(shape.Clone()); }
	size_t GetShapeCount() const									{ return _shapes.GetSize(); }
	CollisionShape& GetShape(size_t index)							{ return *_shapes[index]; }
	const CollisionShape& GetShape(size_t index) const				{ return *_shapes[index]; }
	
	void RemoveShape(size_t index) 
	{ 
		delete _shapes[index]; 
		_shapes.RemoveIndex(index);
	}

	void SetChannels(ECollisionChannels channels)					{ _channels = channels; }
	void AddChannels(ECollisionChannels channels)					{ _channels |= channels; }
	void RemoveChannels(ECollisionChannels channels)				{ _channels = (ECollisionChannels)(_channels & (~channels)); }
	bool CanCollideWithChannels(ECollisionChannels channels) const	{ return (_channels & channels) != 0; }
	bool CanCollideWith(const Collider& other) const				{ return CanCollideWithChannels(other._channels); }

	bool IntersectsRay(const Transform& transform, const Ray&, ECollisionChannels rayChannels, RaycastResult&) const;

	EOverlapResult Overlaps(
		const Transform& transform, 
		const Collider& other, const Transform& otherTransform, 
		const LineSegment* lineA, Vector3* out_PenetrationVector) const;
	
	float MinimumDistanceTo(
		const Transform& transform, 
		const Collider& other, const Transform& otherTransform, 
		Vector3& out_PointA, Vector3& out_PointB, 
		const LineSegment* lineA = nullptr) const;

	Pair<Vector3> GetShallowContactPoints(
		const Transform& transform, float shrink,
		const Collider& other, const Transform& otherTransform, float otherShrink) const;
};
