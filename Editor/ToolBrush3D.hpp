#pragma once
#include "Tool.hpp"
#include <Engine/OBrush3D.hpp>

class ToolBrush3D : public Tool
{
	bool _placing;

	String _materialName;
	void _P_SetMaterialName(const String& materialName);
	const String& _P_GetMaterialName() const { return _materialName; }

	OBrush3D* _object;

	const PropertyCollection& _GetProperties();
public:
	ToolBrush3D(Editor &level) : Tool(level), _placing(false), _object(nullptr) {}
	virtual ~ToolBrush3D() {}

	virtual void Initialise() override;

	virtual void Activate(UIContainer& properties, UIContainer& toolProperties) override;
	virtual void Deactivate() override;

	virtual void Cancel() override;

	virtual void MouseMove(MouseData&) override;
	virtual void MouseDown(MouseData&) override;
	virtual void KeySubmit() override;

	virtual void Render(RenderQueue&) const override;
};

