#pragma once
#include "Entity.hpp"

class EntConnector : public Entity
{
	Vector3 _point1;
	Vector3 _point2;

	void _OnTransformChanged();
	void _OnPointChanged();

	bool _updatingTransform;

public:
	Entity_FUNCS(EntConnector, EEntityID::LEVEL_CONNECTOR)

	bool connected;

	EntConnector() : Entity(EFlags::SAVEABLE), connected(false), _updatingTransform(false) 
	{ 
		onTransformChanged += Callback(this, &EntConnector::_OnTransformChanged); 
	}

	EntConnector(const EntConnector& other) : Entity(other), _point1(other._point1), _point2(other._point2), connected(other.connected), _updatingTransform(other._updatingTransform)
	{
		onTransformChanged += Callback(this, &EntConnector::_OnTransformChanged);
	}

	virtual ~EntConnector() {}

	const Vector3& GetPoint1() const { return _point1; }
	const Vector3& GetPoint2() const { return _point2; }

	void SetPoint1(const Vector3& position) { _point1 = position; _OnPointChanged(); }
	void SetPoint2(const Vector3& position) { _point2 = position; _OnPointChanged(); }

	virtual void Render(RenderQueue&) const;

	virtual Bounds GetBounds() const override
	{
		static Bounds bounds(Vector3(.5f, .5f, .5f));
		return bounds;
	}
};
