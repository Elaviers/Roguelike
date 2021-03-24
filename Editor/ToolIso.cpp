#include "ToolIso.hpp"
#include "Editor.hpp"
#include "UIPropertyManipulator.hpp"
#include <Engine/TileManager.hpp>
#include <ELCore/TextProvider.hpp>
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

//Todo: combo boxes that can store text properly (i.e. not string conversions of text)

String ToolIso::_GetModeName() const
{
	switch (_mode)
	{
	case Mode::ADD:
		return _textAdd.ToString();
	case Mode::MOVE:
		return _textMove.ToString();
	case Mode::REMOVE:
		return _textDelete.ToString();
	case Mode::REMOVEZ:
		return _textDeleteOnLevel.ToString();
	}

	return "???";
}

void ToolIso::_SetModeName(const String& name)
{
	if (name == _textAdd.ToString())
		SetMode(Mode::ADD);
	else if (name == _textMove.ToString())
		SetMode(Mode::MOVE);
	else if (name == _textDelete.ToString())
		SetMode(Mode::REMOVE);
	else if (name == _textDeleteOnLevel.ToString())
		SetMode(Mode::REMOVEZ);
	else
		Debug::Message("ToolIso::_SetModeName was given an unsupported name..", "Good job!");
}

String ToolIso::_GetTileName() const
{
	return _owner.engine.pTileManager->FindNameOf(_gTarget->GetTile().Ptr());
}

void ToolIso::_SetTileName(const String& name)
{
	if (_gTarget)
		_gTarget->SetTile(_owner.engine.pTileManager->Get(name, _owner.engine.context));
}

void ToolIso::_UpdatePlacementTransform()
{
	if (_gTarget)
		_gTarget->SetPositionSize(_pos, _size);
}

void ToolIso::_PlaceTile()
{
	if (_mode == Mode::REMOVEZ)
	{
		for (Geometry* g : _target->GetGeometry())
		{
			GeoIsoTile* tile = dynamic_cast<GeoIsoTile*>(g);
			if (tile && tile->GetPosition() == _pos)
			{
				_target->DeleteGeometry(g);
				return;
			}
		}
	}
	else if (_mode == Mode::ADD)
	{
		for (Geometry* g : _target->GetGeometry())
		{
			if (g != _gTarget)
			{
				GeoIsoTile* tile = dynamic_cast<GeoIsoTile*>(g);
				if (tile->GetPosition() == _gTarget->GetPosition())
				{
					*tile = *_gTarget;
					return;
				}
			}
		}

		GeoIsoTile* newPlacement = _target->CreateGeometry<GeoIsoTile>();
		*newPlacement = *_gTarget;
		_gTarget = newPlacement;
	}
	else if (_mode == Mode::MOVE && _gTarget)
	{
		Vector3 pos = _gTarget->GetPosition();
		pos.x = _pos.x + _pos.y - pos.y;
		pos.z = _pos.z + _pos.y - pos.y;
		_gTarget->SetPosition(pos);
	}
}

void ToolIso::_DeleteHoverTile(MouseData& md)
{
	if (_mode == Mode::REMOVE)
	{
		if (_dragging && *md.hoverObject == *_target)
		{
			Geometry* hoverGeometry = _target->DecodeIDMapValue(md.hoverData[0], md.hoverData[1]);

			if (hoverGeometry)
				_target->DeleteGeometry(hoverGeometry);
		}

		return;
	}
}

void ToolIso::Initialise()
{
	_SetSize(Vector2(1.f, 1.f));

	_textAdd = _owner.engine.pTextProvider->Get("tool_iso_add");
	_textMove = _owner.engine.pTextProvider->Get("tool_iso_move");
	_textDelete = _owner.engine.pTextProvider->Get("tool_iso_delete");
	_textDeleteOnLevel = _owner.engine.pTextProvider->Get("tool_iso_deleteonlevel");
}

void ToolIso::Activate(UIContainer& properties, UIContainer& toolProperties)
{
	if (!_sTarget)
	{
		_target = _owner.WorldRef().FindFirstObjectOfType<OGeometryCollection>();

		if (_target == nullptr)
		{
			_target = _owner.WorldRef().CreateObject<OGeometryCollection>();
			_target->SetName(Text("Geometry"));
		}

		_sTarget = _owner.WorldRef().TrackObject(_target);
	}

	SetMode(Mode::REMOVE);
	SetMode(Mode::ADD); //bit silly, but does the job fine

	//Properties here done manually for the combobox..
	float ih = Editor::PROPERTY_HEIGHT;
	(new UIPropertyManipulator(UICoord(1.f, -ih), ih, _owner, *_GetProperties().Find("Mode"), this, &toolProperties, 
		{ 
			_textAdd, 
			_textMove,
			_textDelete,
			_textDeleteOnLevel
		}))->SetZ(-1.f);

	auto p = new UIPropertyManipulator(UICoord(1.f, -2.f * ih), ih, _owner, *_GetProperties().Find("Level"), this, &toolProperties);
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
		ts->SetBounds(UIBounds(c * w, UICoord(1.f, -y), w, UICoord(0.f, h)));
		ts->onPressed += Function(*this, &ToolIso::TileSelected);
		_tileSelectors.Add(ts);

		if (++c >= columns)
		{
			c = 0;
			y += h + 4.f;
		}
	}
}

void ToolIso::Deactivate()
{
	if (_gTarget)
		_target->DeleteGeometry(_gTarget);

	_gTarget = nullptr;
}

void ToolIso::MouseMove(MouseData& mouseData)
{
	if (_mode == Mode::REMOVE)
	{
		if (_dragging)
			_DeleteHoverTile(mouseData);

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

void ToolIso::MouseDown(MouseData& mouseData)
{
	if (mouseData.viewport && mouseData.viewport->GetCameraType() == Viewport::ECameraType::ISOMETRIC)
	{
		_dragging = true;
		
		if (_mode == Mode::REMOVE)
			_DeleteHoverTile(const_cast<MouseData&>(mouseData));
		else if (_mode == Mode::MOVE)
		{
			if (mouseData.hoverObject == _target)
			{
				Geometry* hoverGeometry = _target->DecodeIDMapValue(mouseData.hoverData[0], mouseData.hoverData[1]);
				_gTarget = dynamic_cast<GeoIsoTile*>(hoverGeometry);
			}
		}
		else
			_PlaceTile();
	}
}

void ToolIso::MouseUp(MouseData& mouseData)
{
	_dragging = false;

	if (_mode == Mode::MOVE)
		_gTarget = nullptr;
}

void ToolIso::Render(RenderQueue& q) const
{
	if (_mode == Mode::REMOVE || (_mode == Mode::MOVE && _gTarget == nullptr))
		return;

	RenderEntry& box = q.CreateEntry(ERenderChannels::UNLIT);
	box.AddSetLineWidth(2.f);
	box.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
	box.AddCommand(RCMDSetDepthFunc::ALWAYS);

	Vector3 pos = _pos;
	if (_mode == Mode::MOVE)
		pos = _gTarget->GetPosition();

	int startY = Maths::Min(0, (int)pos.y);
	int endY = Maths::Max(0, (int)pos.y);
	Vector2 sz = (_gTarget && _gTarget->GetTile()) ? (_size * _gTarget->GetTile()->GetSize()) : _size;

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
	_lastSelectedTile = selector.GetTile();
	_gTarget->SetTile(_lastSelectedTile);
}

void ToolIso::SetMode(Mode mode)
{
	if (_mode == mode) return;

	switch (_mode)
	{
	case Mode::ADD:
		_target->DeleteGeometry(_gTarget);
		break;
	}

	_mode = mode;
	_gTarget = nullptr;

	switch (_mode)
	{
	case Mode::ADD:
		_gTarget = _target->CreateGeometry<GeoIsoTile>();
		_gTarget->SetTile(_lastSelectedTile);
		break;
	}
}

void ToolIso::SetTarget(OGeometryCollection* geometry)
{
	_sTarget = _owner.WorldRef().TrackObject(geometry);
	_target = dynamic_cast<OGeometryCollection*>(_sTarget.Ptr());
}
