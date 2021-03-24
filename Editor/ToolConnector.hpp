#pragma once
#include "Tool.hpp"
#include <Engine/OConnector.hpp>

class ToolConnector : public Tool
{
	bool _placing;

	OConnector* _connector;

	const PropertyCollection& _GetProperties();
public:
	ToolConnector(Editor &level) : Tool(level), _placing(false), _connector(nullptr) {}
	virtual ~ToolConnector() {}

	virtual void Activate(UIContainer& properties, UIContainer& toolProperties) override;
	virtual void Deactivate() override;

	virtual void Cancel() override;

	virtual void MouseMove(MouseData&) override;
	virtual void MouseDown(MouseData&) override;
	virtual void KeySubmit() override;

	virtual void Render(RenderQueue&) const override;
};
