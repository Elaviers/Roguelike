#include "ToolConnector.h"
#include "Editor.h"
#include "EditorUtil.h"

String ToolConnector::GetConnectorDirection() const
{
	switch (_connector.direction)
	{
	case Direction2D::NORTH:
		return "north";
	case Direction2D::EAST:
		return "east";
	case Direction2D::WEST:
		return "south";
	default:
		return "west";
	}
}

void ToolConnector::SetConnectorDirection(const String &dir)
{
	if (dir == "north")
		_connector.direction = Direction2D::NORTH;
	else if (dir == "east")
		_connector.direction = Direction2D::EAST;
	else if (dir == "south")
		_connector.direction = Direction2D::SOUTH;
	else
		_connector.direction = Direction2D::WEST;
}

void ToolConnector::Initialise()
{
	_cvars.Add("Direction", Getter<String>(this, &ToolConnector::GetConnectorDirection), Setter<String>(this, &ToolConnector::SetConnectorDirection), PropertyFlags::DIRECTION);

	_connector.SetRenderColour(Vector4(0.f, 1.f, 0.f, .5f));
}

void ToolConnector::Activate(PropertyWindow &properties, PropertyWindow &toolProperties)
{
	toolProperties.SetCvars(_cvars);

}

void ToolConnector::Cancel()
{
	_placing = false;
}

void ToolConnector::MouseMove(const MouseData &mouseData)
{
	if (_owner.CameraRef(mouseData.viewport).GetProjectionType() == ProjectionType::ORTHOGRAPHIC)
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
	if (_owner.CameraRef(mouseData.viewport).GetProjectionType() == ProjectionType::ORTHOGRAPHIC) _placing = true;
}

void ToolConnector::KeySubmit()
{
	_placing = false;

	ObjConnector *clone = _connector.TypedClone();
	clone->SetRenderColour(Vector4(0.f, 1.f, 0.f, 1.f));
	clone->SetParent(&_owner.LevelRef());

	_connector.SetMin(Vector3());
	_connector.SetMax(Vector3());
}

void ToolConnector::Render() const
{
	glLineWidth(3);
	_connector.Render();
}
