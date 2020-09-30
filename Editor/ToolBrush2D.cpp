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
		ContextualMemberGetter<EntBrush2D, String>(&EntBrush2D::GetMaterialName),
		ContextualMemberSetter<EntBrush2D, String>(&EntBrush2D::SetMaterial),
		offsetof(ToolBrush2D, _object),
		PropertyFlags::MATERIAL);

	properties.Add(
		"Level",
		MemberGetter<EntBrush2D, float>(&EntBrush2D::GetLevel),
		MemberSetter<EntBrush2D, float>(&EntBrush2D::SetLevel),
		offsetof(ToolBrush2D, _object));
	DO_ONCE_END;

	return properties;
}

void ToolBrush2D::Initialise()
{
	_object.SetRelativeScale(Vector3());
	_object.SetMaterial("bricks", _owner.engine.context);
}

void ToolBrush2D::Activate(UIContainer& properties, UIContainer& toolProperties)
{
	UIPropertyManipulator::AddPropertiesToContainer(1.f, Editor::PROPERTY_HEIGHT, _owner, _object.GetProperties(), &_object, properties);
	UIPropertyManipulator::AddPropertiesToContainer(1.f, Editor::PROPERTY_HEIGHT, _owner, _GetProperties(), this, toolProperties);
}

void ToolBrush2D::Cancel()
{
	_placing = false;
}

void ToolBrush2D::MouseMove(const MouseData &mouseData)
{
	if (mouseData.viewport && mouseData.viewport->gridAxis == EAxis::Y)
	{
		Vector2 offset = mouseData.viewport->GetCameraType() == Viewport::ECameraType::ISOMETRIC ? Vector2(-_object.GetLevel(), -_object.GetLevel()) : Vector2();

		if (mouseData.isLeftDown)
		{
			Vector2 p1, p2;
			EditorUtil::CalculatePointsFromMouseData(mouseData, p1, p2);

			_object.SetPoint1(p1 + offset);
			_object.SetPoint2(p2 + offset);
			_owner.RefreshProperties();
		}
		else
		{
			_object.SetPoint1(Vector2((float)mouseData.unitX_rounded, (float)mouseData.unitY_rounded) + offset);
			_object.SetPoint2(Vector2((float)(mouseData.unitX_rounded + 1), (float)(mouseData.unitY_rounded + 1)) + offset);
			_owner.RefreshProperties();
		}
	}
}

void ToolBrush2D::MouseDown(const MouseData &mouseData)
{
	if (mouseData.viewport && mouseData.viewport->gridAxis == EAxis::Y)
		_placing = true;
}

void ToolBrush2D::MouseUp(const MouseData &mouseData)
{
	if (mouseData.viewport && mouseData.viewport->gridAxis == EAxis::Y)
	{
		_placing = false;

		_object.Clone()->SetParent(&_owner.WorldRef().RootEntity());
	}
}

void ToolBrush2D::Render(RenderQueue& q) const
{
	RenderEntry& box = q.CreateEntry(ERenderChannels::UNLIT);
	box.AddSetLineWidth(2.f);
	box.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
	box.AddSetColour(Colour::Green);
	box.AddBox(_object.GetWorldBounds().min, _object.GetWorldBounds().max);

	_object.Render(q);
}
