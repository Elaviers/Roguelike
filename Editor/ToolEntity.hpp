#pragma once
#include "Tool.hpp"
#include <Engine/SharedPointer.hpp>

class ToolEntity : public Tool
{
	byte _classID;

	EntityPointer _placement;
	bool _readyToPlace;

	const PropertyCollection& _GetProperties();

	void _SetClassID(const byte &id);
	byte _GetClassID() const { return _classID; }

public:
	ToolEntity(Editor &level) : Tool(level), _classID(0), _placement(), _readyToPlace(false) {}
	virtual ~ToolEntity() {}

	virtual void Initialise() override;
	virtual void Activate(PropertyWindow &properties, PropertyWindow &toolProperties) override;
	virtual void Deactivate() override;
	virtual void MouseMove(const MouseData&) override;
	virtual void MouseDown(const MouseData&) override;

	virtual void Render(EnumRenderChannel) const override;
};
