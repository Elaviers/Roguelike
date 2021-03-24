#pragma once
#include "Tool.hpp"
#include "UITileSelector.hpp"
#include <Engine/GeoIsoTile.hpp>
#include <ELCore/Text.hpp>

class ToolIso : public Tool
{
public:
	enum class Mode
	{
		ADD,
		MOVE,
		REMOVE,
		REMOVEZ
	} _mode;

protected:
	bool _dragging;

	Vector3 _pos;
	Vector2 _size;

	SharedPointer<WorldObject> _sTarget;
	OGeometryCollection* _target;
	GeoIsoTile* _gTarget;

	List<UITileSelector*> _tileSelectors;
	SharedPointer<const Tile> _lastSelectedTile;

	Text _textAdd;
	Text _textMove;
	Text _textDelete;
	Text _textDeleteOnLevel;

	const PropertyCollection& _GetProperties();

	String _GetModeName() const;
	String _GetTileName() const;
	const Vector2& _GetSize() const { return _size; }

	void _SetModeName(const String& mode);
	void _SetTileName(const String& material);
	void _SetSize(const Vector2& size) 
	{ 
		_size = size; 
	}

	void _UpdatePlacementTransform();

	void _PlaceTile();
	void _DeleteHoverTile(MouseData&);
public:
	ToolIso(Editor& owner) : Tool(owner), _mode(Mode::ADD), _dragging(false) {}
	virtual ~ToolIso() {}

	virtual void Initialise() override;

	virtual void Activate(UIContainer& properties, UIContainer& toolProperties) override;
	virtual void Deactivate() override;

	virtual void MouseMove(MouseData&) override;
	virtual void MouseDown(MouseData&) override;
	virtual void MouseUp(MouseData&) override;

	virtual void Render(RenderQueue&) const override;

	void TileSelected(UITileSelector& selector);

	void SetMode(Mode);

	void SetTarget(OGeometryCollection* gometry);
};
