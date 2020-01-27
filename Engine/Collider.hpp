#pragma once
#include "Buffer.hpp"
#include "CollisionChannels.hpp"
#include "CollisionShape.hpp"

struct LineSegment;
struct RaycastResult;

class Collider
{
	Buffer<CollisionShape*> _shapes;

	CollisionChannels _channels;

public:
	Collider(CollisionChannels channels, const CollisionShape& shape) : _channels(channels), _shapes(shape.Clone()) {}

	Collider(CollisionChannels channels, const Buffer<CollisionShape>& shapes = Buffer<CollisionShape>()) : _channels(channels)
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

	const CollisionChannels& GetChannels() const { return _channels; }

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

	void SetChannels(CollisionChannels channels)					{ _channels = channels; }
	void AddChannels(CollisionChannels channels)					{ _channels |= channels; }
	void RemoveChannels(CollisionChannels channels)					{ _channels = (CollisionChannels)(_channels & (~channels)); }
	bool CanCollideWithChannels(CollisionChannels channels) const	{ return (_channels & channels) != 0; }
	bool CanCollideWith(const Collider& other) const				{ return CanCollideWithChannels(other._channels); }

	bool IntersectsRay(const Transform& transform, const Ray&, RaycastResult&) const;

	bool Overlaps(
		const Transform& transform, 
		const Collider& other, const Transform& otherTransform, 
		const LineSegment* lineA = nullptr) const;
	
	float MinimumDistanceTo(
		const Transform& transform, 
		const Collider& other, const Transform& otherTransform, 
		Vector3& out_PointA, Vector3& out_PointB, 
		const LineSegment* lineA = nullptr) const;
};
