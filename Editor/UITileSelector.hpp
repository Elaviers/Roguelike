#pragma once
#include <ELUI/Container.hpp>
#include <ELUI/Rect.hpp>
#include <ELUI/Panel.hpp>
#include <Engine/Tile.hpp>

class UITileSelector : public UIContainer
{
	UIRect _rect;
	UIPanel _panel;

	SharedPointer<const Tile> _tile;

	UIColour _panelColour;
	UIColour _panelColourHover;
	UIColour _panelColourSelected;

	bool _hold;
	bool _selected;

	virtual void _OnBoundsChanged() override;

public:
	Event<UITileSelector&> onPressed;

	UITileSelector();
	virtual ~UITileSelector() {}

	const SharedPointer<const Tile>& GetTile() const { return _tile; }
	void SetTile(const SharedPointer<const Tile>& tile);

	float GetPanelBorderSize() const { return _panel.GetBorderSize(); }
	const SharedPointer<const Material>& GetPanelMaterial() const { return _panel.GetMaterial(); }
	const UIColour& GetPanelColour() const { return _panelColour; }
	const UIColour& GetPanelColourHover() const { return _panelColourHover; }
	const UIColour& GetPanelColourSelected() const { return _panelColourSelected; }

	bool GetSelected() const { return _selected; }

	UITileSelector& SetPanelBorderSize(float size) { _panel.SetBorderSize(size); _OnBoundsChanged(); return *this; }
	UITileSelector& SetPanelColour(const UIColour& colour) { _panelColour = colour; if (!_hover && !_selected) _panel.SetColour(colour); return *this; }
	UITileSelector& SetPanelColourHover(const UIColour& colour) { _panelColourHover = colour; if (_hover && !_selected) _panel.SetColour(colour); return *this; }
	UITileSelector& SetPanelColourSelected(const UIColour& colour) { _panelColourSelected = colour; if (_selected) _panel.SetColour(colour); return *this; }
	UITileSelector& SetPanelMaterial(const SharedPointer<const Material>& material) { _panel.SetMaterial(material); return *this; }

	UITileSelector& SetSelected(bool selected) 
	{ 
		if (selected != _selected)
		{
			_selected = selected;

			if (_selected)
				_panel.SetColour(_panelColourSelected);
			else
				_panel.SetColour(_hover ? _panelColourHover : _panelColour);
		}

		return *this;
	}

	virtual void OnHoverStart() override;
	virtual void OnHoverStop() override;

	virtual bool OnKeyDown(bool blocked, EKeycode) override;
	virtual bool OnKeyUp(bool blocked, EKeycode) override;
	virtual bool OnMouseMove(bool blocked, float x, float y) override;
};
