#pragma once
#include "Entity.hpp"

class EntConnector : public Entity
{
	static Collider COLLIDER;

public:
	Entity_FUNCS(EntConnector, EEntityID::LEVEL_CONNECTOR)

	bool connected;

	EntConnector() : Entity(EFlags::SAVEABLE), connected(false) { }
	virtual ~EntConnector() {}

	virtual void Render(ERenderChannels) const;

	virtual Bounds GetBounds() const override {
		return Bounds(Vector3(.5f, .5f, 0.f), Vector3(-.5f, -.5f, 0.f));
	}

	virtual const Collider* GetCollider() const override {
		return &COLLIDER;
	}
};
