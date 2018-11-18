#include "ToolEntity.h"
#include "Editor.h"
#include <Engine/Brush.h>
#include <Engine/Renderable.h>

void ToolEntity::_SetClassName(const String &className)
{
	_className = className;

	_owner.PropertyWindowRef().SetObject(Engine::registry.GetObjectInstanceFromClassName(_className));
}

void ToolEntity::Initialise()
{
	_className = "Renderable";
	_properties.SetBase(*this);
	_properties.Add<ToolEntity, String>("Class", this, &ToolEntity::_GetClassName, &ToolEntity::_SetClassName, PropertyFlags::CLASSNAME);

	//Defaults
	reinterpret_cast<Brush<2>*>(Engine::registry.GetObjectInstanceFromClassName("Brush2D"))->SetMaterial("bricks");
	reinterpret_cast<Brush<3>*>(Engine::registry.GetObjectInstanceFromClassName("Brush3D"))->SetMaterial("bricks");
	reinterpret_cast<Renderable*>(Engine::registry.GetObjectInstanceFromClassName("Renderable"))->SetModel("sphere");
}

void ToolEntity::Activate(PropertyWindow &properties, PropertyWindow &toolProperties)
{
	properties.Clear();
	toolProperties.SetProperties(_properties);

	_owner.PropertyWindowRef().SetObject(Engine::registry.GetObjectInstanceFromClassName(_className));
}

void ToolEntity::MouseDown(const MouseData &mouseData)
{
	if (_owner.CameraRef(mouseData.viewport).GetProjectionType() == ProjectionType::PERSPECTIVE)
	{
		Camera &camera = _owner.CameraRef(mouseData.viewport);
		RECT windowDims;
		::GetClientRect(_owner.ViewportRef(mouseData.viewport).GetHwnd(), &windowDims);
		Ray r(camera.transform.Position(), camera.ScreenCoordsToDirection(Vector2((float)mouseData.x / (float)windowDims.right, (float)mouseData.y / (float)windowDims.bottom)));
		Buffer<RaycastResult> results = _owner.LevelRef().ObjectCollection().Raycast(r);

		if (results.GetSize() > 0)
		{
			GameObject *newObj = Engine::registry.NewObjectFromClassName(_className);
			_owner.LevelRef().ObjectCollection().InsertObject(newObj);

			Vector3 pos = r.origin + r.direction * results[0].entryTime;
			
			_owner.PropertyWindowRef().GetProperties().Apply(newObj);
			pos[1] -= newObj->GetBounds().min[1];
			newObj->transform.SetPosition(pos);
		}
	}
}

