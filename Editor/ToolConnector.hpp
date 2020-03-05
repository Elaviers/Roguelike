#pragma once
#include "Tool.hpp"
#include <Engine/EntConnector.hpp>

class ToolConnector : public Tool
{
	bool _placing;

	EntConnector _connector;

	const PropertyCollection& _GetProperties();
public:
	ToolConnector(Editor &level) : Tool(level), _placing(false) {}
	virtual ~ToolConnector() {}

	virtual void Initialise();

	virtual void Activate(UIContainer& properties, UIContainer& toolProperties) override;

	virtual void Cancel() override;

	virtual void MouseMove(const MouseData&) override;
	virtual void MouseDown(const MouseData&) override;
	virtual void KeySubmit() override;

	virtual void Render(ERenderChannels) const override;
};

