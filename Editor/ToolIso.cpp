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
		"Size",
		MemberGetter<ToolIso, const Vector2&>(&ToolIso::_GetSize),
		MemberSetter<ToolIso, Vector2>(&ToolIso::_SetSize));

	DO_ONCE_END;

	return properties;
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
	_placementTile.SetTransform(_pos, _size);
}

void ToolIso::Initialise()
{
	_SetSize(Vector2(1.f, 1.f));
}

void ToolIso::Activate(UIContainer& properties, UIContainer& toolProperties)
{
	UIPropertyManipulator::AddPropertiesToContainer(Editor::PROPERTY_HEIGHT, _owner, _GetProperties(), this, toolProperties);

	_tileSelectors.Clear();

	TileManager* tileManager = _owner.engine.pTileManager;
	Buffer<String> tiles = tileManager->GetAllPossibleKeys();

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
	if (mouseData.viewport && mouseData.viewport->GetCameraType() == Viewport::ECameraType::ISOMETRIC)
	{
		_pos.x = mouseData.unitX_rounded;
		_pos.z = mouseData.unitY_rounded;
		_UpdatePlacementTransform();
	}
}

void ToolIso::MouseDown(const MouseData& mouseData)
{
	_owner.WorldRef().AddGeometry(GeoIsoTile(_placementTile));
}

void ToolIso::Render(RenderQueue& q) const
{
	_placementTile.Render(q);
}

void ToolIso::TileSelected(UITileSelector& selector)
{
	for (UITileSelector* ts : _tileSelectors)
		ts->SetSelected(false);

	selector.SetSelected(true);
	_placementTile.SetTile(selector.GetTile());
}
