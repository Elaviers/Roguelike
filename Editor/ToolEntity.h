#pragma once
#include "Tool.h"
#include <Engine/ObjectProperties.h>

class ToolEntity : public Tool
{
	String _className;

	ObjectProperties _properties;

	void _SetClassName(const String &className);
	String _GetClassName() const { return _className; }

public:
	ToolEntity(Editor &level) : Tool(level) {}
	virtual ~ToolEntity() {}

	virtual void Initialise();
	virtual void Activate(PropertyWindow &properties, PropertyWindow &toolProperties) override;
	virtual void MouseDown(const MouseData&);
};
