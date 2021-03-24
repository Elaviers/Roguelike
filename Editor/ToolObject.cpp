#include "ToolObject.hpp"
#include "Editor.hpp"
#include "UIPropertyManipulator.hpp"
#include <Engine/ORenderable.hpp>
#include <ELCore/MacroUtilities.hpp>

const PropertyCollection& ToolObject::_GetProperties()
{
	static PropertyCollection properties;
	
	DO_ONCE(
		properties.Add(
		"Class",
		MemberGetter<ToolObject, byte>(&ToolObject::_GetClassID),
		MemberSetter<ToolObject, byte>(&ToolObject::_SetClassID),
		0,
		PropertyFlags::CLASSID)
	);

	return properties;
}

void ToolObject::_SetClassID(const byte &id)
{
	_classID = (EObjectID)id;

	if (_placement)
		_placement->Destroy();
	
	_placement = _owner.WorldRef().TrackObject(_owner.engine.objectTypes.GetType(_classID)->New(_owner.WorldRef()));
	
	_owner.ChangePropertyWorldObject(_placement.Ptr());
}

bool ToolObject::_ViewportCanPlace(const Viewport& vp) const
{
	return vp.GetCameraType() == Viewport::ECameraType::PERSPECTIVE || vp.GetCameraType() == Viewport::ECameraType::ISOMETRIC;
}

void ToolObject::Initialise()
{
	_classID = EObjectID::RENDERABLE;
}

void ToolObject::Activate(UIContainer& properties, UIContainer& toolProperties)
{
	UIPropertyManipulator::AddPropertiesToContainer(1.f, Editor::PROPERTY_HEIGHT, _owner, _GetProperties(), this, toolProperties);

	//Creates the placement object
	_SetClassID((byte)_classID);

	_readyToPlace = false;
}

void ToolObject::Deactivate()
{
	if (_placement)
		_placement->Destroy();
	
	_placement.Clear();
}

void ToolObject::MouseMove(MouseData& mouseData)
{
	if (mouseData.viewport && _placement && _ViewportCanPlace(*mouseData.viewport))
	{
		Viewport& vp = *mouseData.viewport;

		Ray r = vp.cameraProjection.ScreenToWorld(vp.cameraTransform, Vector2((float)mouseData.x / vp.cameraProjection.GetDimensions()[0], (float)mouseData.y / vp.cameraProjection.GetDimensions()[1]));
		Buffer<RaycastResult> results = _owner.WorldRef().Raycast(r, ECollisionChannels::ALL);

		RaycastResult* relevantResult = nullptr;
		for (RaycastResult& rr : results)
			if (rr.object != _placement)
			{
				relevantResult = &rr;
				break;
			}

		if (relevantResult)
		{
			Vector3 pos = r.origin + r.direction * relevantResult->time;

			if (const Volume* v = _placement->GetVolume())
			{
				Vector3 min, max;
				v->Bounds_AABB(_placement->GetAbsoluteTransform(), min, max);
				pos.y += _placement->GetAbsolutePosition().y - min.y;
			}

			_placement->SetRelativePosition(pos);
		
			_owner.RefreshProperties();

			_readyToPlace = true;
			return;
		}
	}

	_readyToPlace = false;
}

void ToolObject::MouseDown(MouseData &mouseData)
{
	if (_placement && _readyToPlace)
		_placement = _owner.WorldRef().TrackObject(_owner.WorldRef().CloneObject(*_placement, false));
}

void ToolObject::Render(RenderQueue& q) const
{
	if (_placement && _readyToPlace)
	{
		const Volume* v = _placement->GetVolume();
		if (v)
		{
			RenderEntry& box = q.CreateEntry(ERenderChannels::UNLIT);
			box.AddSetLineWidth(2.f);
			box.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
			box.AddSetColour(Colour::Green);
		
			Vector3 min, max;
			v->Bounds_AABB(_placement->GetAbsoluteTransform(), min, max);
			box.AddBox(min, max);
		}

		_placement->Render(q);
	}
}
