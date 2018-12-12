#pragma once
#include "Tool.h"
#include <Engine/Brush3D.h>
#include <Engine/ObjectProperties.h>

class ToolBrush3D : public Tool
{
	bool _placing;

	Brush3D _object;
	ObjectProperties _properties;

public:
	ToolBrush3D(Editor &level) : Tool(level) {}
	virtual ~ToolBrush3D() {}

	virtual void Initialise();

	virtual void Activate(PropertyWindow &properties, PropertyWindow &toolProperties) override;

	virtual void Cancel() override;

	virtual void MouseMove(const MouseData&) override;
	virtual void MouseDown(const MouseData&) override;
	virtual void KeySubmit() override;

	virtual void Render() const override;
};

