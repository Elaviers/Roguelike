#include "ToolConnector.hpp"
#include "Editor.hpp"
#include "EditorUtil.hpp"
#include "UIPropertyManipulator.hpp"
#include <ELCore/MacroUtilities.hpp>

const PropertyCollection& ToolConnector::_GetProperties()
{
	static PropertyCollection properties;

	return properties;
}

void ToolConnector::Activate(UIContainer& properties, UIContainer& toolProperties)
{
	_connector = _owner.WorldRef().CreateObject<OConnector>();
	
	UIPropertyManipulator::AddPropertiesToContainer(1.f, Editor::PROPERTY_HEIGHT, _owner, _GetProperties(), this, toolProperties);
}

void ToolConnector::Deactivate()
{
	_connector->Destroy();
}

void ToolConnector::Cancel()
{
	_placing = false;
}

void ToolConnector::MouseMove(MouseData &mouseData)
{
	/* old
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

			Vector3 v;

			v[rightElement] = p1[0];
			v[upElement] = p1[1];
			v[fwdElement] = _connector.GetPoint1()[fwdElement];
			_connector.SetPoint1(v);

			v[rightElement] = p2[0];
			v[upElement] = p2[1];
			v[fwdElement] = _connector.GetPoint2()[fwdElement];
			_connector.SetPoint2(v);
		}
		else if (!_placing)
		{
			Vector3 v;

			v[rightElement] = (float)mouseData.unitX_rounded;
			v[upElement] = (float)mouseData.unitY_rounded;
			v[fwdElement] = -100.f;
			_connector.SetPoint1(v);

			v[rightElement] = (float)mouseData.unitX_rounded + 1.f;
   			v[upElement] = (float)mouseData.unitY_rounded + 1.f;
			v[fwdElement] = 100.f;
			_connector.SetPoint2(v);
		}
	}
	*/
}

void ToolConnector::MouseDown(MouseData &mouseData)
{
	if (mouseData.viewport && mouseData.viewport->cameraProjection.GetType() == EProjectionType::ORTHOGRAPHIC) _placing = true;
}

void ToolConnector::KeySubmit()
{
	_placing = false;

	_connector = _owner.WorldRef().CreateObject<OConnector>();
}

void ToolConnector::Render(RenderQueue& q) const
{
	q.CreateEntry(ERenderChannels::EDITOR).AddPushColourOverride(Colour(0.f, 1.f, 0.f));
	_connector->Render(q);
	q.CreateEntry(ERenderChannels::EDITOR).AddPopColourOverride();
}
