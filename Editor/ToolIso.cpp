#include "ToolIso.hpp"
#include "Editor.hpp"
#include "UIPropertyManipulator.hpp"
#include <Engine/TileManager.hpp>
#include <ELPhys/Collision.hpp>

const PropertyCollection& ToolIso::_GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE_BEGIN;
	properties.Add(
		"Mode",
		MemberGetter<ToolIso, String>(&ToolIso::_GetModeName),
		MemberSetter<ToolIso, String>(&ToolIso::_SetModeName)
		);

	properties.Add<float>(
		"Level",
		offsetof(ToolIso, _pos.y)
	);

	properties.Add(
		"Size",
		MemberGetter<ToolIso, const Vector2&>(&ToolIso::_GetSize),
		MemberSetter<ToolIso, Vector2>(&ToolIso::_SetSize));

	DO_ONCE_END;

	return properties;
}

String ToolIso::_GetModeName() const
{
	switch (_mode)
	{
	case Mode::ADD:
		return "Add";
	case Mode::MOVE:
		return "Move";
	case Mode::REMOVE:
		return "Delete";
	case Mode::REMOVEZ:
		return "Delete on level";
	}

	return "???";
}

void ToolIso::_SetModeName(const String& name)
{
	String l = name.ToLower();
	if (l == "add")
		_mode = Mode::ADD;
	else if (l == "move")
		_mode = Mode::MOVE;
	else if (l == "delete")
		_mode = Mode::REMOVE;
	else if (l == "delete on level")
		_mode = Mode::REMOVEZ;
	else
		Debug::Message("ToolIso::_SetModeName was given an invalid name..", "Good job!");
}

String ToolIso::_GetTileName() const
{
	return _owner.engine.pTileManager->FindNameOf(_placementTile.GetTile().Ptr());
}

void ToolIso::_SetTileName(const String& name)
{
	_placementTile.SetTile(_owner.engine.pTileManager->Get(name, _owner.engine.context));
}

void ToolIso::_UpdatePlacementTransform()
{
	_placementTile.SetPositionSize(_pos, _size);
}

void ToolIso::_PlaceTile()
{
	if (_mode == Mode::REMOVEZ)
	{
		for (Geometry* g : _owner.WorldRef().GetGeometry())
		{
			GeoIsoTile* tile = dynamic_cast<GeoIsoTile*>(g);
			if (tile->GetPosition() == _placementTile.GetPosition())
			{
				_owner.WorldRef().RemoveGeometry(g->GetUID());
				return;
			}
		}
	}
	else if (_mode == Mode::ADD)
	{
		for (Geometry* g : _owner.WorldRef().GetGeometry())
		{
			GeoIsoTile* tile = dynamic_cast<GeoIsoTile*>(g);
			if (tile->GetPosition() == _placementTile.GetPosition())
			{
				*tile = _placementTile;
				return;
			}
		}

		_owner.WorldRef().AddGeometry(GeoIsoTile(_placementTile));
	}
	else if (_mode == Mode::MOVE && _dragGeometry)
	{
		Vector3 pos = _dragGeometry->GetPosition();
		pos.x = _pos.x + _pos.y - pos.y;
		pos.z = _pos.z + _pos.y - pos.y;
		_dragGeometry->SetPosition(pos);
	}
}

void ToolIso::_DeleteHoverTile(MouseData& md)
{
	if (_mode == Mode::REMOVE)
	{
		if (_dragging && md.hoverGeometry)
		{
			_owner.WorldRef().RemoveGeometry(md.hoverGeometry->GetUID());
			md.hoverGeometry = nullptr;
		}

		return;
	}
}

void ToolIso::Initialise()
{
	_SetSize(Vector2(1.f, 1.f));
}

void ToolIso::Activate(UIContainer& properties, UIContainer& toolProperties)
{
	//Properties here done manually for the combobox..
	float ih = Editor::PROPERTY_HEIGHT;
	(new UIPropertyManipulator(UICoord(1.f, -ih), ih, _owner, *_GetProperties().Find("Mode"), this, &toolProperties, { "Add", "Move", "Delete", "Delete on level" }))->SetZ(-1.f);
	new UIPropertyManipulator(UICoord(1.f, -2.f * ih), ih, _owner, *_GetProperties().Find("Level"), this, &toolProperties);
	new UIPropertyManipulator(UICoord(1.f, -3.f * ih), ih, _owner, *_GetProperties().Find("Size"), this, &toolProperties);

	_tileSelectors.Clear();

	TileManager* tileManager = _owner.engine.pTileManager;
	Buffer<String> tiles = tileManager->GetAllPossibleKeys(_owner.engine.context);

	SharedPointer<const Material> panelMat = _owner.engine.pMaterialManager->Get("uiarea", _owner.engine.context);

	const int columns = 4;
	const float h = 64.f;
	
	const float w = 1.f / columns;
	int c = 0;
	float y = h;
	for (const String& tilename : tiles)
	{
		UITileSelector* ts = new UITileSelector();
		ts->SetParent(&properties);
		ts->SetPanelBorderSize(8.f)
			.SetPanelMaterial(panelMat)
			.SetPanelColour(UIColour(Colour(.4f, .4f, .4f, .4f), Colour::Black))
			.SetPanelColourHover(UIColour(Colour(.6f, .6f, .6f, .6f), Colour::White))
			.SetPanelColourSelected(UIColour(Colour(0.f, 0.f, 1.f, .5f), Colour::Black))
			.SetTile(tileManager->Get(tilename, _owner.engine.context));
		ts->SetBounds(c * w, UICoord(1.f, -y), w, UICoord(0.f, h));
		ts->onPressed += FunctionPointer(this, &ToolIso::TileSelected);
		_tileSelectors.Add(ts);

		if (++c >= columns)
		{
			c = 0;
			y += h + 4.f;
		}
	}
}

void ToolIso::MouseMove(const MouseData& mouseData)
{
	if (_mode == Mode::REMOVE)
	{
		if (_dragging && mouseData.hoverGeometry)
			_DeleteHoverTile(const_cast<MouseData&>(mouseData)); //TODO TODO TODO... just get a non-const mousedata ref here!!!!

		return;
	}

	if (mouseData.viewport && mouseData.viewport->GetCameraType() == Viewport::ECameraType::ISOMETRIC)
	{
		float newX = mouseData.unitX_rounded - _pos.y;
		float newZ = mouseData.unitY_rounded - _pos.y;

		if (_pos.x != newX || _pos.z != newZ)
		{
			_pos.x = mouseData.unitX_rounded - _pos.y;
			_pos.z = mouseData.unitY_rounded - _pos.y;
			_UpdatePlacementTransform();

			if (_dragging)
				_PlaceTile();
		}
	}
}

void ToolIso::MouseDown(const MouseData& mouseData)
{
	if (mouseData.viewport->GetCameraType() == Viewport::ECameraType::ISOMETRIC)
	{
		_dragging = true;
		_dragGeometry = dynamic_cast<GeoIsoTile*>(mouseData.hoverGeometry);

		if (_mode == Mode::REMOVE)
			_DeleteHoverTile(const_cast<MouseData&>(mouseData)); //TODO TODO TODO... just get a non-const mousedata ref here!!!!
		else
			_PlaceTile();
	}
}

void ToolIso::MouseUp(const MouseData& mouseData)
{
	_dragging = false;
	_dragGeometry = nullptr;
}

void ToolIso::Render(RenderQueue& q) const
{
	if (_mode == Mode::REMOVE || (_mode == Mode::MOVE && _dragGeometry == nullptr))
		return;

	if (_mode == Mode::ADD)
		_placementTile.Render(q);

	RenderEntry& box = q.CreateEntry(ERenderChannels::UNLIT);
	box.AddSetLineWidth(2.f);
	box.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
	box.AddCommand(RCMDSetDepthFunc::ALWAYS);

	Vector3 pos = _pos;
	if (_mode == Mode::MOVE)
		pos = _dragGeometry->GetPosition();

	int startY = Maths::Min(0, (int)pos.y);
	int endY = Maths::Max(0, (int)pos.y);
	Vector2 sz = _placementTile.GetTile() ? (_size * _placementTile.GetTile()->GetSize()) : _size;

	box.AddSetColour(Colour(.1f, .5f, .1f));
	for (int y = startY; y < endY; ++y)
		box.AddBox(Vector3(pos.x, (float)y, pos.z), Vector3(pos.x + sz.x, y + 1.f, pos.z + sz.x));

	Vector3 topCorner = pos + Vector3(sz.x, sz.y, sz.x);

	box.AddSetColour(Colour::Green);
	box.AddBox(Vector3(pos.x, 0.f, pos.z), topCorner);

	if (_mode == Mode::REMOVEZ)
	{
		box.AddSetColour(Colour::Red);
		box.AddBox(pos, pos + Vector3(1.f, 1.f, 1.f));
	}
	else
		box.AddBox(pos, topCorner);

	box.AddCommand(RCMDSetDepthFunc::LEQUAL);
}

void ToolIso::TileSelected(UITileSelector& selector)
{
	for (UITileSelector* ts : _tileSelectors)
		ts->SetSelected(false);

	selector.SetSelected(true);
	_placementTile.SetTile(selector.GetTile());
}
