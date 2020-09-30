#pragma once
#include "Entity.hpp"

class EntConnector : public Entity
{
public:
	Entity_FUNCS(EntConnector, EEntityID::LEVEL_CONNECTOR)

	bool connected;

	EntConnector() : Entity(EFlags::SAVEABLE), connected(false) {}
	virtual ~EntConnector() {}

	virtual void Render(RenderQueue&) const;

	virtual Bounds GetBounds() const override
	{
		static Bounds bounds(Vector3(.5f, .5f, .5f));
		return bounds;
	}
};
