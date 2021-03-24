#pragma once
#include "WorldObject.hpp"

class OConnector : public WorldObject
{
public:
	WORLDOBJECT_VFUNCS(OConnector, EObjectID::LEVEL_CONNECTOR)

	bool connected;

protected:
	OConnector(World& world) : WorldObject(world), connected(false) {}
	
public:
	virtual ~OConnector() {}

	virtual void Render(RenderQueue&) const;

	virtual bool IsVisible(const Frustum& view) const override;
};
