#include "ToolEntity.hpp"
#include "Editor.hpp"
#include <Engine/ObjBrush.hpp>
#include <Engine/ObjRenderable.hpp>
#include <Engine/RaycastResult.hpp>
#include <Engine/Registry.hpp>

void ToolEntity::_SetClassID(const byte &id)
{
	_classID = id;

	if (_placement)
		_placement->Delete();

	_placement = Engine::Instance().pObjectTracker->Track(Engine::Instance().registry.GetNode(_classID)->New());

	_owner.PropertyWindowRef().SetObject(_placement.Ptr());
}

void ToolEntity::Initialise()
{
	_classID = 1;
	_cvars.Add("Class", Getter<byte>(this, &ToolEntity::_GetClassID), Setter<byte>(this, &ToolEntity::_SetClassID), CvarFlags::CLASSID);
}

void ToolEntity::Activate(PropertyWindow &properties, PropertyWindow &toolProperties)
{
	properties.Clear();
	toolProperties.SetCvars(_cvars);

	//Creates the placement object
	_SetClassID(_classID);

	_readyToPlace = false;
}

void ToolEntity::Deactivate()
{
	if (_placement)
		_placement->Delete();
	
	_placement.Clear();
}

void ToolEntity::MouseMove(const MouseData& mouseData)
{
	if (_placement && _owner.CameraRef(mouseData.viewport).GetProjectionType() == ProjectionType::PERSPECTIVE)
	{
		ObjCamera& camera = _owner.CameraRef(mouseData.viewport);
		RECT windowDims;
		::GetClientRect(_owner.ViewportRef(mouseData.viewport).GetHwnd(), &windowDims);
		Ray r = camera.ScreenCoordsToRay(Vector2((float)mouseData.x / (float)windowDims.right, (float)mouseData.y / (float)windowDims.bottom));
		Buffer<RaycastResult> results = _owner.LevelRef().Raycast(r);

		if (results.GetSize() > 0)
		{
			Vector3 pos = r.origin + r.direction * results[0].entryTime;
			pos[1] -= _placement->GetWorldBounds(true).min[1];
			_placement->SetRelativePosition(pos);
		
			_readyToPlace = true;
			return;
		}
	}

	_readyToPlace = false;
}

void ToolEntity::MouseDown(const MouseData &mouseData)
{
	if (_placement && _readyToPlace)
	{
		GameObject* newObj = Engine::Instance().registry.GetNode(_classID)->New();
		newObj->SetParent(&_owner.LevelRef());

		CvarMap newObjCvars;
		newObj->GetCvars(newObjCvars);
		_owner.PropertyWindowRef().GetCvars().TransferValuesTo(newObjCvars);
	}
}

void ToolEntity::Render(EnumRenderChannel channels) const
{
	if (_placement && _readyToPlace)
	{
		GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, Vector4(1.f, 1.f, 1.f, 0.5f));
		_placement->Render(channels);
	}
}
