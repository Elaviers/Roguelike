#pragma once
#include "Tool.h"
#include <Engine/Brush2D.h>
#include <Engine/ObjectProperties.h>

class ToolBrush2D : public Tool
{
	bool _placing;

	Brush2D _object;
	ObjectProperties _properties;

public:
	ToolBrush2D(Editor &owner) : Tool(owner) {}
	virtual ~ToolBrush2D() {}

	virtual void Initialise();

	virtual void Activate(PropertyWindow &properties, PropertyWindow &toolProperties) override;

	virtual void Cancel() override;

	virtual void MouseMove(const MouseData&) override;
	virtual void MouseDown(const MouseData&) override;
	virtual void MouseUp(const MouseData&) override;

	virtual void Render() const override;
};

