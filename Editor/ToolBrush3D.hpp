#pragma once
#include "Tool.hpp"
#include <Engine/EntBrush3D.hpp>

class ToolBrush3D : public Tool
{
	bool _placing;

	EntBrush3D _object;

	const PropertyCollection& _GetProperties();
public:
	ToolBrush3D(Editor &level) : Tool(level), _placing(false) {}
	virtual ~ToolBrush3D() {}

	virtual void Initialise();

	virtual void Activate(UIContainer& properties, UIContainer& toolProperties) override;

	virtual void Cancel() override;

	virtual void MouseMove(const MouseData&) override;
	virtual void MouseDown(const MouseData&) override;
	virtual void KeySubmit() override;

	virtual void Render(ERenderChannels) const override;
};

