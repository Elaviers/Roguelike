#include "ToolBrush3D.hpp"
#include "Editor.hpp"
#include "EditorUtil.hpp"
#include "UIPropertyManipulator.hpp"
#include <ELCore/MacroUtilities.hpp>

const PropertyCollection& ToolBrush3D::_GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE(
		properties.Add(
		"Material",
		ContextualMemberGetter<EntBrush3D, String>(&EntBrush3D::GetMaterialName),
		ContextualMemberSetter<EntBrush3D, String>(&EntBrush3D::SetMaterial),
		offsetof(ToolBrush3D, _object),
		PropertyFlags::MATERIAL)
	);

	return properties;
}

void ToolBrush3D::Initialise()
{
	_object.SetRelativeScale(Vector3());
	_object.SetMaterial("alt", _owner.engine.context);
}

void ToolBrush3D::Activate(UIContainer& properties, UIContainer& toolProperties)
{
	UIPropertyManipulator::AddPropertiesToContainer(Editor::PROPERTY_HEIGHT, _owner, _object.GetProperties(), &_object, properties);
	UIPropertyManipulator::AddPropertiesToContainer(Editor::PROPERTY_HEIGHT, _owner, _GetProperties(), this, toolProperties);
}

void ToolBrush3D::Cancel()
{
	_placing = false;
}

void ToolBrush3D::MouseMove(const MouseData &mouseData)
{
	if (mouseData.viewport && mouseData.viewport->camera.GetProjection().GetType() == EProjectionType::ORTHOGRAPHIC)
	{
		EAxis fwdAxis = mouseData.viewport->gridAxis;
		int fwdElement = (int)fwdAxis;
		int rightElement = (int)Axes::GetHorizontalAxis(fwdAxis);
		int upElement = (int)Axes::GetVerticalAxis(fwdAxis);

		if (mouseData.isLeftDown)
		{
			Vector2 p1, p2;
			EditorUtil::CalculatePointsFromMouseData(mouseData, p1, p2);

			Vector3 p13d = _object.GetPoint1();
			Vector3 p23d = _object.GetPoint2();

			p13d[rightElement] = (float)p1[0];
			p13d[upElement] = (float)p1[1];
			p23d[rightElement] = (float)p2[0];
			p23d[upElement] = (float)p2[1];

			_object.SetPoint1(p13d);
			_object.SetPoint2(p23d);
			_owner.RefreshProperties();
		}
		else if (!_placing)
		{
			Vector3 p1;
			p1[rightElement] = (float)mouseData.unitX_rounded;
			p1[upElement] = (float)mouseData.unitY_rounded;
			p1[fwdElement] = -100;

			Vector3 p2;
			p2[rightElement] = (float)(mouseData.unitX_rounded + 1);
			p2[upElement] = (float)(mouseData.unitY_rounded + 1);
			p2[fwdElement] = 100;

			_object.SetPoint1(p1);
			_object.SetPoint2(p2);
			_owner.RefreshProperties();
		}
	}
}

void ToolBrush3D::MouseDown(const MouseData &mouseData)
{
	if (mouseData.viewport && mouseData.viewport->camera.GetProjection().GetType() == EProjectionType::ORTHOGRAPHIC) _placing = true;
}

void ToolBrush3D::KeySubmit()
{
	_placing = false;

	_object.Clone()->SetParent(&_owner.WorldRef().RootEntity());
}

void ToolBrush3D::Render(RenderQueue& q) const
{
	RenderEntry& box = q.NewDynamicEntry(ERenderChannels::UNLIT);
	box.AddSetLineWidth(2.f);
	box.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
	box.AddSetColour(Colour::Green);
	box.AddBox(_object.GetWorldBounds().min, _object.GetWorldBounds().max);

	_object.Render(q);
}
