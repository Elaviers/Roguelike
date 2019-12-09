#include "ToolEntity.hpp"
#include "Editor.hpp"
#include <Engine/EntBrush.hpp>
#include <Engine/EntRenderable.hpp>
#include <Engine/MacroUtilities.hpp>
#include <Engine/RaycastResult.hpp>
#include <Engine/Registry.hpp>

const PropertyCollection& ToolEntity::_GetProperties()
{
	static PropertyCollection properties;
	
	DO_ONCE(
		properties.Add(
		"Class",
		MemberGetter<ToolEntity, byte>(&ToolEntity::_GetClassID),
		MemberSetter<ToolEntity, byte>(&ToolEntity::_SetClassID),
		0,
		PropertyFlags::CLASSID)
	);

	return properties;
}

void ToolEntity::_SetClassID(const byte &id)
{
	_classID = EntityID(id);

	if (_placement)
		_placement->Delete();

	_placement = Engine::Instance().pObjectTracker->Track(Engine::Instance().registry.GetNode(_classID)->New());

	_owner.PropertyWindowRef().SetObject(_placement.Ptr());
}

void ToolEntity::Initialise()
{
	_classID = EntityID::RENDERABLE;
}

void ToolEntity::Activate(PropertyWindow &properties, PropertyWindow &toolProperties)
{
	properties.Clear();
	toolProperties.SetCvars(_GetProperties(), this);

	//Creates the placement object
	_SetClassID((byte)_classID);

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
		EntCamera& camera = _owner.CameraRef(mouseData.viewport);
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
		Entity* newObj = Engine::Instance().registry.GetNode(_classID)->New();
		newObj->SetParent(&_owner.LevelRef());

		const PropertyCollection& cvars = newObj->GetProperties();
		_owner.PropertyWindowRef().GetProperties()->Transfer(_placement.Ptr(), newObj);
	}
}

void ToolEntity::Render(RenderChannels channels) const
{
	if (_placement && _readyToPlace)
	{
		GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, Colour(1.f, 1.f, 1.f, 0.5f));
		_placement->Render(channels);
	}
}
