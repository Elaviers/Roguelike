#pragma once
#include "Tool.h"
#include <Engine/Connector.h>
#include <Engine/ObjectProperties.h>

class ToolConnector : public Tool
{
	bool _placing;

	Connector _connector;
	ObjectProperties _properties;

public:
	ToolConnector(Editor &level) : Tool(level) {}
	virtual ~ToolConnector() {}

	virtual void Initialise();

	virtual void Activate(PropertyWindow &properties, PropertyWindow &toolProperties) override;

	virtual void Cancel() override;

	virtual void MouseMove(const MouseData&) override;
	virtual void MouseDown(const MouseData&) override;
	virtual void KeySubmit() override;

	virtual void Render() const override;

	String GetConnectorDirection() const;
	void SetConnectorDirection(const String &dir);
};

