#pragma once
#include "Tool.hpp"
#include <Engine/GeoIsoTile.hpp>

class ToolIso : public Tool
{
protected:
	Vector3 _pos;
	Vector2 _size;

	GeoIsoTile _placementTile;

	const PropertyCollection& _GetProperties();

	String _GetMaterial() const;
	const Vector2& _GetSize() const { return _size; }

	void _SetMaterial(const String& material);
	void _SetSize(const Vector2& size) 
	{ 
		_size = size; 
	}

	void _UpdatePlacementTransform();
public:
	ToolIso(Editor& owner) : Tool(owner) {}
	virtual ~ToolIso() {}

	virtual void Initialise() override;

	virtual void Activate(UIContainer& properties, UIContainer& toolProperties) override;

	virtual void MouseMove(const MouseData&) override;
	virtual void MouseDown(const MouseData&) override;

	virtual void Render(RenderQueue&) const override;
};
