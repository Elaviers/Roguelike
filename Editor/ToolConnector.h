#pragma once
#include "Tool.h"
#include <Engine/Connector.h>

class ToolConnector : public Tool
{
	Connector _connector;

	bool _placing;

public:
	ToolConnector(Editor &level) : Tool(level) {}
	virtual ~ToolConnector() {}

	virtual void MouseMove(const MouseData&) override;
	virtual void MouseDown(const MouseData&) override;
	virtual void KeySubmit() override;

	virtual void Render() const override;
};

