#pragma once
#include "Tool.hpp"
#include "UITileSelector.hpp"
#include <Engine/GeoIsoTile.hpp>

class ToolIso : public Tool
{
protected:
	bool _dragging;

	Vector3 _pos;
	Vector2 _size;

	GeoIsoTile _placementTile;

	List<UITileSelector*> _tileSelectors;

	const PropertyCollection& _GetProperties();

	String _GetTileName() const;
	const Vector2& _GetSize() const { return _size; }

	void _SetTileName(const String& material);
	void _SetSize(const Vector2& size) 
	{ 
		_size = size; 
	}

	void _UpdatePlacementTransform();
public:
	ToolIso(Editor& owner) : Tool(owner), _dragging(false) {}
	virtual ~ToolIso() {}

	virtual void Initialise() override;

	virtual void Activate(UIContainer& properties, UIContainer& toolProperties) override;

	virtual void MouseMove(const MouseData&) override;
	virtual void MouseDown(const MouseData&) override;
	virtual void MouseUp(const MouseData&) override;

	virtual void Render(RenderQueue&) const override;

	void TileSelected(UITileSelector& selector);
};
