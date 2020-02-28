#include "ToolEntity.hpp"
#include "Editor.hpp"
#include "UIPropertyManipulator.hpp"
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
	_classID = id;

	if (_placement)
		_placement->Delete();

	_placement = Engine::Instance().pObjectTracker->Track(Engine::Instance().registry.GetNode(_classID)->New());

	_owner.ChangePropertyEntity(_placement.Ptr());
}

void ToolEntity::Initialise()
{
	_classID = (byte)EEntityID::RENDERABLE;
}

void ToolEntity::Activate(UIContainer& properties, UIContainer& toolProperties)
{
	UIPropertyManipulator::AddPropertiesToContainer(Editor::PROPERTY_HEIGHT, _owner, _GetProperties(), this, toolProperties);

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
	if (mouseData.viewport >= 0 && _placement && _owner.GetVP(mouseData.viewport).camera.GetProjectionType() == EProjectionType::PERSPECTIVE)
	{
		EntCamera& camera = _owner.GetVP(mouseData.viewport).camera;
		Ray r = camera.ScreenCoordsToRay(Vector2((float)mouseData.x / camera.GetViewport()[1], (float)mouseData.y / camera.GetViewport()[0]));
		Buffer<RaycastResult> results = _owner.LevelRef().Raycast(r);

		if (results.GetSize() > 0)
		{
			Vector3 pos = r.origin + r.direction * results[0].entryTime;
			pos[1] -= _placement->GetWorldBounds(true).min[1];
			_placement->SetRelativePosition(pos);
		
			_owner.RefreshProperties();

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

		Entity* from = (Entity*)(_owner.GetPropertyObject());
		if (from)
		{
			const PropertyCollection& cvars = newObj->GetProperties();
			cvars.Transfer(from, newObj);
		}
	}
}

void ToolEntity::Render(ERenderChannels channels) const
{
	if (_placement && _readyToPlace)
	{
		GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, Colour(1.f, 1.f, 1.f, 0.5f));
		_placement->Render(channels);
	}
}
