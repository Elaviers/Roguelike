#include "ToolEntity.hpp"
#include "Editor.hpp"
#include "UIPropertyManipulator.hpp"
#include <Engine/EntRenderable.hpp>
#include <Engine/Registry.hpp>
#include <ELCore/MacroUtilities.hpp>
#include <ELPhys/RaycastResult.hpp>

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
		_placement->Delete(_owner.engine.context);
	
	_placement = _owner.engine.pObjectTracker->Track(_owner.engine.registry.GetNode(_classID)->New());

	_owner.ChangePropertyEntity(_placement.Ptr());
}

bool ToolEntity::_ViewportCanPlace(const Viewport& vp) const
{
	return vp.GetCameraType() == Viewport::ECameraType::PERSPECTIVE || vp.GetCameraType() == Viewport::ECameraType::ISOMETRIC;
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
		_placement->Delete(_owner.engine.context);
	
	_placement.Clear();
}

void ToolEntity::MouseMove(const MouseData& mouseData)
{
	if (mouseData.viewport && _placement && _ViewportCanPlace(*mouseData.viewport))
	{
		EntCamera& camera = mouseData.viewport->camera;
		Ray r = camera.GetProjection().ScreenToWorld(camera.GetWorldTransform(), Vector2((float)mouseData.x / camera.GetProjection().GetDimensions()[0], (float)mouseData.y / camera.GetProjection().GetDimensions()[1]));
		Buffer<RaycastResult> results = _owner.LevelRef().Raycast(r);

		if (results.GetSize() > 0)
		{
			Vector3 pos = r.origin + r.direction * results[0].entryTime;
			pos.y -= _placement->GetWorldBounds(true).min.y;
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
		Entity* newObj = _owner.engine.registry.GetNode(_classID)->New();
		newObj->SetParent(&_owner.LevelRef());

		Entity* from = (Entity*)(_owner.GetPropertyObject());
		if (from)
		{
			const PropertyCollection& cvars = newObj->GetProperties();
			cvars.Transfer(from, newObj, _owner.engine.context);
		}

		newObj = newObj;
	}
}

void ToolEntity::Render(RenderQueue& q) const
{
	if (_placement && _readyToPlace)
	{
		_placement->Render(q);
	}
}
