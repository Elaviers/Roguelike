#pragma once
#include "Tool.hpp"
#include <Engine/OBrush2D.hpp>

class ToolBrush2D : public Tool
{
	bool _placing;

	String _materialName;
	void _P_SetMaterialName(const String& materialName);
	const String& _P_GetMaterialName() const { return _materialName; }

	float _level;
	void _P_SetLevel(const float& level);
	float _P_GetLevel() const { return _level; }

	OBrush2D* _object;

	const PropertyCollection& _GetProperties();
public:
	ToolBrush2D(Editor &owner) : Tool(owner), _placing(false), _object(nullptr) {}
	virtual ~ToolBrush2D() {}

	virtual void Initialise() override;

	virtual void Activate(UIContainer& properties, UIContainer& toolProperties) override;
	virtual void Deactivate() override;

	virtual void Cancel() override;

	virtual void MouseMove(MouseData&) override;
	virtual void MouseDown(MouseData&) override;
	virtual void MouseUp(MouseData&) override;

	virtual void Render(RenderQueue&) const override;
};

