#include "ToolIso.hpp"
#include "Editor.hpp"
#include "UIPropertyManipulator.hpp"
#include <ELPhys/Collision.hpp>

const PropertyCollection& ToolIso::_GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE_BEGIN;
	properties.Add(
		"Material",
		MemberGetter<ToolIso, String>(&ToolIso::_GetMaterial),
		MemberSetter<ToolIso, String>(&ToolIso::_SetMaterial),
		0,
		PropertyFlags::MATERIAL);

	properties.Add(
		"Size",
		MemberGetter<ToolIso, const Vector2&>(&ToolIso::_GetSize),
		MemberSetter<ToolIso, Vector2>(&ToolIso::_SetSize));

	DO_ONCE_END;

	return properties;
}

String ToolIso::_GetMaterial() const
{
	return _owner.engine.pMaterialManager->FindNameOf(_placementTile.GetMaterial().Ptr());
}

void ToolIso::_SetMaterial(const String& name)
{
	_placementTile.SetMaterial(_owner.engine.pMaterialManager->Get(name, _owner.engine.context));
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
