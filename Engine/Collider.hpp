#pragma once
#include "Buffer.hpp"
#include "CollisionChannels.hpp"
#include "CollisionShape.hpp"
#include "CollisionBox.hpp"
#include "CollisionSphere.hpp"

struct RaycastResult;

class Collider
{
public:
	class AnyCollisionShape
	{
		CollisionShape* _shape;

	public:
		AnyCollisionShape() : _shape(nullptr) {}
		AnyCollisionShape(const CollisionSphere& sphere) { _shape = new CollisionSphere(sphere); }
		AnyCollisionShape(const CollisionBox& box) { _shape = new CollisionBox(box); }

		AnyCollisionShape(const AnyCollisionShape& other) : _shape(nullptr)
		{
			if (other._shape)
			{
				switch (other._shape->GetType())
				{
				case CollisionType::BOX:
					_shape = new CollisionBox((const CollisionBox&)*other._shape);
					break;
				case CollisionType::SPHERE:
					_shape = new CollisionSphere((const CollisionSphere&)*other._shape);
					break;
				}
			}
		}

		~AnyCollisionShape() 
		{
			delete _shape;
		}

		CollisionShape& Get() { return *_shape; }
		const CollisionShape& Get() const { return *_shape; }

		void operator=(const CollisionShape&) = delete;
	};

private:
	Buffer<AnyCollisionShape> _shapes;

	CollisionChannels _channels;

	struct GJKResult
	{
		const CollisionShape* hitShape;
		float distance;
	};

	GJKResult _GJK(const Transform& transform, const Collider& other, const Transform &otherTransform) const;

public:
	Collider(CollisionChannels channels, const CollisionBox& box) : _channels(channels), _shapes(box) {}
	Collider(CollisionChannels channels, const CollisionSphere& sphere) : _channels(channels), _shapes(sphere) {}

	Collider(CollisionChannels channels, const Buffer<AnyCollisionShape>& shapes = Buffer<AnyCollisionShape>()) : _channels(channels), _shapes(shapes) {}

	const CollisionChannels& GetChannels() const { return _channels; }

	template<typename T>
	T& AddShape(const T& shape)							{ return dynamic_cast<T&>(_shapes.Add(shape).Get()); }
	size_t GetShapeCount() const						{ return _shapes.GetSize(); }
	CollisionShape& GetShape(size_t index)				{ return _shapes[index].Get(); }
	const CollisionShape& GetShape(size_t index) const	{ return _shapes[index].Get(); }
	void RemoveShape(size_t index)						{ _shapes.RemoveIndex(index); }

	void SetChannels(CollisionChannels channels)					{ _channels = channels; }
	void AddChannels(CollisionChannels channels)					{ _channels |= channels; }
	void RemoveChannels(CollisionChannels channels)					{ _channels = (CollisionChannels)(_channels & (~channels)); }
	bool CanCollideWithChannels(CollisionChannels channels) const	{ return (_channels & channels) != 0; }
	bool CanCollideWith(const Collider& other) const				{ return CanCollideWithChannels(other._channels); }

	bool IntersectsRay(const Transform& transform, const Ray&, RaycastResult&) const;

	bool Overlaps(const Transform& transform, const CollisionShape& other, const Transform& otherTransform) const;
	bool Overlaps(const Transform& transform, const Collider &other, const Transform &otherTransform) const;
};
