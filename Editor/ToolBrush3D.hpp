#pragma once
#include "Tool.h"
#include <Engine/CvarMap.hpp>
#include <Engine/ObjBrush3D.hpp>

class ToolBrush3D : public Tool
{
	bool _placing;

	ObjBrush3D _object;
	CvarMap _cvars;

public:
	ToolBrush3D(Editor &level) : Tool(level), _placing(false) {}
	virtual ~ToolBrush3D() {}

	virtual void Initialise();

	virtual void Activate(PropertyWindow &properties, PropertyWindow &toolProperties) override;

	virtual void Cancel() override;

	virtual void MouseMove(const MouseData&) override;
	virtual void MouseDown(const MouseData&) override;
	virtual void KeySubmit() override;

	virtual void Render() const override;
};

