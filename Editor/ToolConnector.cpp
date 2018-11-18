#include "ToolConnector.h"
#include "Editor.h"
#include "EditorUtil.h"

void ToolConnector::MouseMove(const MouseData &mouseData)
{
	if (mouseData.isLeftDown)
	{
		Vector2 p1, p2;
		EditorUtil::CalculatePointsFromMouseData(mouseData, p1, p2);

		_connector.point1[mouseData.rightElement] = p1[0];
		_connector.point1[mouseData.upElement] = p1[1];
		_connector.point2[mouseData.rightElement] = p2[0];
		_connector.point2[mouseData.upElement] = p2[1];
	}
	else if (!_placing)
	{
		_connector.point1[mouseData.rightElement] = mouseData.unitX_rounded;
		_connector.point1[mouseData.upElement] = mouseData.unitY_rounded;
		_connector.point1[mouseData.forwardElement] = -100;
		_connector.point2[mouseData.rightElement] = mouseData.unitX_rounded + 1;
		_connector.point2[mouseData.upElement] = mouseData.unitY_rounded + 1;
		_connector.point2[mouseData.forwardElement] = 100;
	}
}

void ToolConnector::MouseDown(const MouseData &mouseData)
{
	if (_owner.CameraRef(mouseData.viewport).GetProjectionType() == ProjectionType::ORTHOGRAPHIC) _placing = true;
}

void ToolConnector::KeySubmit()
{
	_placing = false;

	_owner.LevelRef().Connectors().Add(_connector);
	_connector.point1 = _connector.point2 = Vector<int16, 3>(0, 0, 0);
}

void ToolConnector::Render() const
{
	glLineWidth(3);
	GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, Vector4(0.f, 1.f, 0.f, 1.f));
	_connector.Render();
}

