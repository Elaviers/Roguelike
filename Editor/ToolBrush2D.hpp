#pragma once
#include "Tool.hpp"
#include <Engine/CvarMap.hpp>
#include <Engine/ObjBrush2D.hpp>

class ToolBrush2D : public Tool
{
	bool _placing;

	ObjBrush2D _object;
	CvarMap _cvars;

public:
	ToolBrush2D(Editor &owner) : Tool(owner), _placing(false) {}
	virtual ~ToolBrush2D() {}

	virtual void Initialise();

	virtual void Activate(PropertyWindow &properties, PropertyWindow &toolProperties) override;

	virtual void Cancel() override;

	virtual void MouseMove(const MouseData&) override;
	virtual void MouseDown(const MouseData&) override;
	virtual void MouseUp(const MouseData&) override;

	virtual void Render(EnumRenderChannel) const override;
};

