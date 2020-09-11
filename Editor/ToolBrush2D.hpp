#pragma once
#include "Tool.hpp"
#include <Engine/EntBrush2D.hpp>

class ToolBrush2D : public Tool
{
	bool _placing;

	EntBrush2D _object;

	const PropertyCollection& _GetProperties();
public:
	ToolBrush2D(Editor &owner) : Tool(owner), _placing(false) {}
	virtual ~ToolBrush2D() {}

	virtual void Initialise() override;

	virtual void Activate(UIContainer& properties, UIContainer& toolProperties) override;

	virtual void Cancel() override;

	virtual void MouseMove(const MouseData&) override;
	virtual void MouseDown(const MouseData&) override;
	virtual void MouseUp(const MouseData&) override;

	virtual void Render(RenderQueue&) const override;
};

