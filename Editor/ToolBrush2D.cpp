#include "ToolBrush2D.hpp"
#include "Editor.hpp"
#include "EditorUtil.hpp"
#include "UIPropertyManipulator.hpp"
#include <ELCore/MacroUtilities.hpp>

const PropertyCollection& ToolBrush2D::_GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE_BEGIN;
	properties.Add(
		"Material",
		MemberGetter<ToolBrush2D, const String&>(&ToolBrush2D::_P_GetMaterialName),
		MemberSetter<ToolBrush2D, String>(&ToolBrush2D::_P_SetMaterialName),
		0,
		PropertyFlags::MATERIAL);

	properties.Add(
		"Level",
		MemberGetter<ToolBrush2D, float>(&ToolBrush2D::_P_GetLevel),
		MemberSetter<ToolBrush2D, float>(&ToolBrush2D::_P_SetLevel));
	DO_ONCE_END;

	return properties;
}

void ToolBrush2D::_P_SetMaterialName(const String& materialName)
{
	_materialName = materialName;

	if (_object)
		_object->SetMaterial(materialName, _owner.engine.context);
}

void ToolBrush2D::_P_SetLevel(const float& level)
{
	_level = level;

	if (_object)
		_object->SetLevel(_level);
}

void ToolBrush2D::Initialise()
{
	_materialName = "bricks";
	_level = 0;
}

void ToolBrush2D::Activate(UIContainer& properties, UIContainer& toolProperties)
{
	_object = _owner.WorldRef().CreateObject<OBrush2D>();
	_object->SetRelativeScale(Vector3());
	_object->SetMaterial(_materialName, _owner.engine.context);
	_object->SetLevel(_level);

	UIPropertyManipulator::AddPropertiesToContainer(1.f, Editor::PROPERTY_HEIGHT, _owner, _object->GetProperties(), _object, properties);
	UIPropertyManipulator::AddPropertiesToContainer(1.f, Editor::PROPERTY_HEIGHT, _owner, _GetProperties(), this, toolProperties);
}

void ToolBrush2D::Deactivate()
{
	_object->Destroy();
}

void ToolBrush2D::Cancel()
{
	_placing = false;

	_object = _owner.WorldRef().CreateObject<OBrush2D>();
}

void ToolBrush2D::MouseMove(MouseData &mouseData)
{
	if (mouseData.viewport && mouseData.viewport->gridAxis == EAxis::Y)
	{
		Vector2 offset = mouseData.viewport->GetCameraType() == Viewport::ECameraType::ISOMETRIC ? Vector2(-_object->GetLevel(), -_object->GetLevel()) : Vector2();

		if (mouseData.isLeftDown)
		{
			Vector2 p1, p2;
			EditorUtil::CalculatePointsFromMouseData(mouseData, p1, p2);

			_object->SetPoint1(p1 + offset);
			_object->SetPoint2(p2 + offset);
			_owner.RefreshProperties();
		}
		else
		{
			_object->SetPoint1(Vector2((float)mouseData.unitX_rounded, (float)mouseData.unitY_rounded) + offset);
			_object->SetPoint2(Vector2((float)(mouseData.unitX_rounded + 1), (float)(mouseData.unitY_rounded + 1)) + offset);
			_owner.RefreshProperties();
		}
	}
}

void ToolBrush2D::MouseDown(MouseData &mouseData)
{
	if (mouseData.viewport && mouseData.viewport->gridAxis == EAxis::Y)
		_placing = true;
}

void ToolBrush2D::MouseUp(MouseData &mouseData)
{
	if (mouseData.viewport && mouseData.viewport->gridAxis == EAxis::Y)
	{
		_placing = false;

		_object = _owner.WorldRef().CreateObject<OBrush2D>();
	}
}

void ToolBrush2D::Render(RenderQueue& q) const
{
	RenderEntry& box = q.CreateEntry(ERenderChannels::UNLIT);
	box.AddSetLineWidth(2.f);
	box.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
	box.AddSetColour(Colour::Green);

	Vector3 min, max;
	_object->GetVolume()->Bounds_AABB(_object->GetAbsoluteTransform(), min, max);
	box.AddBox(min, max);

	_object->Render(q);
}
