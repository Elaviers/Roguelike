#include "ToolEntity.hpp"
#include "Editor.hpp"
#include <Engine/ObjBrush.hpp>
#include <Engine/ObjRenderable.hpp>
#include <Engine/RaycastResult.hpp>
#include <Engine/Registry.hpp>

void ToolEntity::_SetClassID(const byte &id)
{
	_classID = id;

	_owner.PropertyWindowRef().SetObject(Engine::Instance().registry.GetNode(_classID)->Object());
}

void ToolEntity::Initialise()
{
	_classID = 1;
	_cvars.Add("Class", Getter<byte>(this, &ToolEntity::_GetClassID), Setter<byte>(this, &ToolEntity::_SetClassID), PropertyFlags::CLASSID);

	//Defaults
	Registry &reg = Engine::Instance().registry;

	reg.GetFirstNodeOfType<ObjBrush2D>()->object.SetMaterial("bricks");
	reg.GetFirstNodeOfType<ObjBrush3D>()->object.SetMaterial("bricks");
	reg.GetFirstNodeOfType<ObjRenderable>()->object.SetModel("sphere");
}

void ToolEntity::Activate(PropertyWindow &properties, PropertyWindow &toolProperties)
{
	properties.Clear();
	toolProperties.SetCvars(_cvars);

	_owner.PropertyWindowRef().SetObject(Engine::Instance().registry.GetNode(_classID)->Object());
}

void ToolEntity::MouseDown(const MouseData &mouseData)
{
	if (_owner.CameraRef(mouseData.viewport).GetProjectionType() == ProjectionType::PERSPECTIVE)
	{
		ObjCamera &camera = _owner.CameraRef(mouseData.viewport);
		RECT windowDims;
		::GetClientRect(_owner.ViewportRef(mouseData.viewport).GetHwnd(), &windowDims);
		Ray r = camera.ScreenCoordsToRay(Vector2((float)mouseData.x / (float)windowDims.right, (float)mouseData.y / (float)windowDims.bottom));
		Buffer<RaycastResult> results = _owner.LevelRef().Raycast(r);

		if (results.GetSize() > 0)
		{
			GameObject *newObj = Engine::Instance().registry.GetNode(_classID)->New();
			newObj->SetParent(&_owner.LevelRef());

			CvarMap newObjCvars;
			newObj->GetCvars(newObjCvars);
			_owner.PropertyWindowRef().GetCvars().TransferValuesTo(newObjCvars);

			Vector3 pos = r.origin + r.direction * results[0].entryTime;
			pos[1] -= newObj->GetBounds().min[1];
			newObj->SetRelativePosition(pos);
		}
	}
}

