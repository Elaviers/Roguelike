#include "ToolBrush2D.h"
#include "Editor.h"
#include "EditorUtil.h"
#include <Engine/ObjectProperties.h>

void ToolBrush2D::Initialise()
{
	_object.transform.SetScale(Vector3());
	_object.SetMaterial("bricks");
	_properties.SetBase(_object);
	_properties.Add<Brush2D, String>("Material", &_object, &Brush2D::GetMaterialName, &Brush2D::SetMaterial, PropertyFlags::MATERIAL);
	_properties.Add<float>("Level", _object.level);
}

void ToolBrush2D::Activate(PropertyWindow &properties, PropertyWindow &toolProperties)
{
	properties.SetObject(&_object, true);
	toolProperties.SetProperties(_properties);
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

		GameObject *brush = new Brush2D(_object);
 
		_owner.LevelRef().ObjectCollection().InsertObject(brush);
	}
}

void ToolBrush2D::Render() const
{
	GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, Vector4(.8f, .8f, .8f, .5f));
	_object.Render();
}
