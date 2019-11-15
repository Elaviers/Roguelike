#include "ToolBrush2D.hpp"
#include "Editor.hpp"
#include "EditorUtil.hpp"
#include <Engine/MacroUtilities.hpp>

const PropertyCollection& ToolBrush2D::_GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE_BEGIN;
	properties.Add(
		"Material",
		MemberGetter<EntBrush<2>, String>(&EntBrush<2>::GetMaterialName),
		MemberSetter<EntBrush<2>, String>(&EntBrush<2>::SetMaterial),
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

void ToolBrush2D::Activate(PropertyWindow &properties, PropertyWindow &toolProperties)
{
	properties.SetObject(&_object, true);
	toolProperties.SetCvars(_GetProperties(), this);
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
		}
		else
		{
			_object.SetPoint1(Vector2((float)mouseData.unitX_rounded, (float)mouseData.unitY_rounded));
			_object.SetPoint2(Vector2((float)(mouseData.unitX_rounded + 1), (float)(mouseData.unitY_rounded + 1)));
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

void ToolBrush2D::Render(EnumRenderChannel channels) const
{
	GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, Colour(.8f, .8f, .8f, .5f));
	_object.Render(channels);
}
