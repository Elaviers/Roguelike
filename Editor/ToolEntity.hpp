#pragma once
#include "Tool.h"
#include <Engine/CvarMap.hpp>

class ToolEntity : public Tool
{
	byte _classID;

	CvarMap _cvars;

	void _SetClassID(const byte &id);
	byte _GetClassID() const { return _classID; }

public:
	ToolEntity(Editor &level) : Tool(level), _classID(0) {}
	virtual ~ToolEntity() {}

	virtual void Initialise() override;
	virtual void Activate(PropertyWindow &properties, PropertyWindow &toolProperties) override;
	virtual void MouseDown(const MouseData&) override;
};
