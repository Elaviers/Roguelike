#include "ToolBrush2D.h"
#include "Editor.h"
#include "EditorUtil.h"

void ToolBrush2D::Initialise()
{
	_object.SetRelativeScale(Vector3());
	_object.SetMaterial("bricks");

	_cvars.Add("Material", Getter<String>((ObjBrush<2>*)&_object, &ObjBrush<2>::GetMaterialName), Setter<String>((ObjBrush<2>*)&_object, &ObjBrush<2>::SetMaterial), PropertyFlags::MATERIAL);
	_cvars.Add("Level", _object.level);
}

void ToolBrush2D::Activate(PropertyWindow &properties, PropertyWindow &toolProperties)
{
	properties.SetObject(&_object, true);
	toolProperties.SetCvars(_cvars);
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

void ToolBrush2D::Render() const
{
	GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, Vector4(.8f, .8f, .8f, .5f));
	_object.Render();
}
