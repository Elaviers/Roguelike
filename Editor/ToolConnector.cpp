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

void ToolConnector::Initialise()
{
	//_connector.SetRenderColour(Colour(0.f, 1.f, 0.f, .5f));
}

void ToolConnector::Activate(UIContainer& properties, UIContainer& toolProperties)
{
	UIPropertyManipulator::AddPropertiesToContainer(Editor::PROPERTY_HEIGHT, _owner, _GetProperties(), this, toolProperties);
}

void ToolConnector::Cancel()
{
	_placing = false;
}

void ToolConnector::MouseMove(const MouseData &mouseData)
{
	if (mouseData.viewport >= 0 && _owner.GetVP(mouseData.viewport).camera.GetProjection().GetType() == EProjectionType::ORTHOGRAPHIC)
	{
		if (mouseData.isLeftDown)
		{
			Vector2 p1, p2;
			EditorUtil::CalculatePointsFromMouseData(mouseData, p1, p2);

			Vector3 v;

			v[mouseData.rightElement] = p1[0];
			v[mouseData.upElement] = p1[1];
			v[mouseData.forwardElement] = _connector.GetPoint1()[mouseData.forwardElement];
			_connector.SetPoint1(v);

			v[mouseData.rightElement] = p2[0];
			v[mouseData.upElement] = p2[1];
			v[mouseData.forwardElement] = _connector.GetPoint2()[mouseData.forwardElement];
			_connector.SetPoint2(v);
		}
		else if (!_placing)
		{
			Vector3 v;

			v[mouseData.rightElement] = (float)mouseData.unitX_rounded;
			v[mouseData.upElement] = (float)mouseData.unitY_rounded;
			v[mouseData.forwardElement] = -100.f;
			_connector.SetPoint1(v);

			v[mouseData.rightElement] = (float)mouseData.unitX_rounded + 1.f;
   			v[mouseData.upElement] = (float)mouseData.unitY_rounded + 1.f;
			v[mouseData.forwardElement] = 100.f;
			_connector.SetPoint2(v);
		}
	}
}

void ToolConnector::MouseDown(const MouseData &mouseData)
{
	if (mouseData.viewport >= 0 && _owner.GetVP(mouseData.viewport).camera.GetProjection().GetType() == EProjectionType::ORTHOGRAPHIC) _placing = true;
}

void ToolConnector::KeySubmit()
{
	_placing = false;

	EntConnector *clone = _connector.TypedClone();
	//clone->SetRenderColour(Colour(0.f, 1.f, 0.f));
	clone->SetParent(&_owner.LevelRef());
}

void ToolConnector::Render(RenderQueue& q) const
{
	_connector.Render(q);
}
