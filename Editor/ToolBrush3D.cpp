#include "ToolBrush3D.hpp"
#include "Editor.hpp"
#include "EditorUtil.hpp"
#include <Engine/MacroUtilities.hpp>

const PropertyCollection& ToolBrush3D::_GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE(
		properties.Add(
		"Material",
		MemberGetter<EntBrush<3>, String>(&EntBrush<3>::GetMaterialName),
		MemberSetter<EntBrush<3>, String>(&EntBrush<3>::SetMaterial),
		offsetof(ToolBrush3D, _object),
		PropertyFlags::MATERIAL)
	);

	return properties;
}

void ToolBrush3D::Initialise()
{
	_object.SetRelativeScale(Vector3());
	_object.SetMaterial("alt");
}

void ToolBrush3D::Activate(PropertyWindow &properties, PropertyWindow &toolProperties)
{
	properties.SetObject(&_object, true);
	toolProperties.SetCvars(_GetProperties(), this);
}

void ToolBrush3D::Cancel()
{
	_placing = false;
}

void ToolBrush3D::MouseMove(const MouseData &mouseData)
{
	if (_owner.CameraRef(mouseData.viewport).GetProjectionType() == ProjectionType::ORTHOGRAPHIC)
	{
		if (mouseData.isLeftDown)
		{
			Vector2 p1, p2;
			EditorUtil::CalculatePointsFromMouseData(mouseData, p1, p2);

			Vector3 p13d = _object.GetPoint1();
			Vector3 p23d = _object.GetPoint2();

			p13d[mouseData.rightElement] = (float)p1[0];
			p13d[mouseData.upElement] = (float)p1[1];
			p23d[mouseData.rightElement] = (float)p2[0];
			p23d[mouseData.upElement] = (float)p2[1];

			_object.SetPoint1(p13d);
			_object.SetPoint2(p23d);
		}
		else if (!_placing)
		{
			Vector3 p1;
			p1[mouseData.rightElement] = (float)mouseData.unitX_rounded;
			p1[mouseData.upElement] = (float)mouseData.unitY_rounded;
			p1[mouseData.forwardElement] = -100;

			Vector3 p2;
			p2[mouseData.rightElement] = (float)(mouseData.unitX_rounded + 1);
			p2[mouseData.upElement] = (float)(mouseData.unitY_rounded + 1);
			p2[mouseData.forwardElement] = 100;

			_object.SetPoint1(p1);
			_object.SetPoint2(p2);
		}
	}
}

void ToolBrush3D::MouseDown(const MouseData &mouseData)
{
	if (_owner.CameraRef(mouseData.viewport).GetProjectionType() == ProjectionType::ORTHOGRAPHIC) _placing = true;
}

void ToolBrush3D::KeySubmit()
{
	_placing = false;

	_object.Clone()->SetParent(&_owner.LevelRef());
}

void ToolBrush3D::Render(RenderChannels channels) const
{
	GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, Colour(.8f, .8f, .8f, .5f));
	_object.Render(channels);
}

