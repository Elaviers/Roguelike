#pragma once
#include "Tool.hpp"
#include <Engine/CvarMap.hpp>
#include <Engine/ObjConnector.hpp>

class ToolConnector : public Tool
{
	bool _placing;

	ObjConnector _connector;
	CvarMap _cvars;

public:
	ToolConnector(Editor &level) : Tool(level), _placing(false) {}
	virtual ~ToolConnector() {}

	virtual void Initialise();

	virtual void Activate(PropertyWindow &properties, PropertyWindow &toolProperties) override;

	virtual void Cancel() override;

	virtual void MouseMove(const MouseData&) override;
	virtual void MouseDown(const MouseData&) override;
	virtual void KeySubmit() override;

	virtual void Render(EnumRenderChannel) const override;

	String GetConnectorDirection() const;
	void SetConnectorDirection(const String &dir);
};

