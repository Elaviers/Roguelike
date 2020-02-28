#include "ToolBrush2D.hpp"
#include "Editor.hpp"
#include "EditorUtil.hpp"
#include "UIPropertyManipulator.hpp"
#include <Engine/MacroUtilities.hpp>

const PropertyCollection& ToolBrush2D::_GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE_BEGIN;
	properties.Add(
		"Material",
		MemberGetter<EntBrush2D, String>(&EntBrush2D::GetMaterialName),
		MemberSetter<EntBrush2D, String>(&EntBrush2D::SetMaterial),
		offsetof(ToolBrush2D, _object),
		PropertyFlags::MATERIAL);

	properties.Add<float>(
		"Level",
		offsetof(ToolBrush2D, _object.level));
	DO_ONCE_END;

	return properties;
}

void ToolBrush2D::Initialise()
{
	_object.SetRelativeScale(Vector3());
	_object.SetMaterial("bricks");
}

void ToolBrush2D::Activate(UIContainer& properties, UIContainer& toolProperties)
{
	UIPropertyManipulator::AddPropertiesToContainer(Editor::PROPERTY_HEIGHT, _owner, _object.GetProperties(), &_object, properties);
	UIPropertyManipulator::AddPropertiesToContainer(Editor::PROPERTY_HEIGHT, _owner, _GetProperties(), this, toolProperties);
}

void ToolBrush2D::Cancel()
{
	_placing = false;
}

void ToolBrush2D::MouseMove(const MouseData &mouseData)
{
	if (mouseData.forwardElement == 1)
	{
		if (mouseData.isLeftDown)
		{
			Vector2 p1, p2;
			EditorUtil::CalculatePointsFromMouseData(mouseData, p1, p2);

			_object.SetPoint1(p1);
			_object.SetPoint2(p2);
			_owner.RefreshProperties();
		}
		else
		{
			_object.SetPoint1(Vector2((float)mouseData.unitX_rounded, (float)mouseData.unitY_rounded));
			_object.SetPoint2(Vector2((float)(mouseData.unitX_rounded + 1), (float)(mouseData.unitY_rounded + 1)));
			_owner.RefreshProperties();
		}
	}
}

void ToolBrush2D::MouseDown(const MouseData &mouseData)
{
	if (mouseData.viewport == 1)
		_placing = true;
}

void ToolBrush2D::MouseUp(const MouseData &mouseData)
{
	if (mouseData.viewport == 1)
	{
		_placing = false;

		_object.Clone()->SetParent(&_owner.LevelRef());
	}
}

void ToolBrush2D::Render(ERenderChannels channels) const
{
	GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, Colour(.8f, .8f, .8f, .5f));
	_object.Render(channels);
}
