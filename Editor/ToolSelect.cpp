#include "ToolSelect.hpp"
#include "Editor.hpp"
#include "EditorUtil.hpp"
#include "UIPropertyManipulator.hpp"
#include <ELCore/MacroUtilities.hpp>
#include <ELGraphics/RenderCommand.hpp>
#include <ELGraphics/RenderEntry.hpp>
#include <ELGraphics/RenderQueue.hpp>
#include <ElSys/InputManager.hpp>
#include <ELSys/System.hpp>
#include <ELPhys/RaycastResult.hpp>

const PropertyCollection& ToolSelect::_GetProperties()
{
	static PropertyCollection properties;

	DO_ONCE_BEGIN;
	properties.Add<float>(
		"GridSnap",
		offsetof(ToolSelect, _gridSnap));

	properties.Add<bool>(
		"SnapToWorld",
		offsetof(ToolSelect, _snapToWorld));

	properties.Add<bool>(
		"GizmoIsLocal",
		offsetof(ToolSelect, _gizmoIsLocal));
	DO_ONCE_END;

	return properties;
}

void ToolSelect::_GizmoMove(const Vector3& delta)
{
	Vector3 d(delta);

	if (!_snapToWorld && _gridSnap)
	{
		d.x = Maths::Trunc(d.x, _gridSnap);
		d.y = Maths::Trunc(d.y, _gridSnap);
		d.z = Maths::Trunc(d.z, _gridSnap);
	}

	for (size_t i = 0; i < _selectedObjects.GetSize(); ++i)
	{
		Vector3 startPos = _selectedObjects[i]->GetWorldPosition();
		Vector3 endPos = startPos + d;

		if (_snapToWorld && _gridSnap)
		{
			endPos.x = Maths::Trunc(endPos.x, _gridSnap);
			endPos.y = Maths::Trunc(endPos.y, _gridSnap);
			endPos.z = Maths::Trunc(endPos.z, _gridSnap);
		}

		if (_shouldCopy && (endPos != startPos))
		{
			_shouldCopy = false;
			_CloneSelection();
		}

		_selectedObjects[i]->SetWorldPosition(endPos);
	}
}

float ToolSelect::_GizmoRotate(const Vector3 &axis, float angle)
{
	Rotation rotation(Quaternion(axis, angle));

	if (rotation.GetEuler().LengthSquared() > 0.f && _shouldCopy)
	{
		_shouldCopy = false;
		_CloneSelection();
	}

	for (size_t i = 0; i < _selectedObjects.GetSize(); ++i)
	{
		_selectedObjects[i]->SetWorldPosition(
			VectorMaths::RotateAbout(_selectedObjects[i]->GetWorldPosition(), _gizmo.GetPosition(), rotation));

		_selectedObjects[i]->SetWorldRotation(_selectedObjects[i]->GetWorldRotation() * rotation);
	}

	return _gizmoIsLocal ? 0.f : angle;
}

void ToolSelect::_UpdateGizmoPos()
{
	if (_gizmoIsLocal && _selectedObjects.GetSize() == 1)
		_gizmo.SetTransform(Transform(_selectedObjects[0]->GetWorldPosition(), _selectedObjects[0]->GetWorldRotation()));
	else
	{
		Vector3 avgPosition;

		for (size_t i = 0; i < _selectedObjects.GetSize(); ++i)
			avgPosition += _selectedObjects[i]->GetWorldPosition();

		avgPosition /= (float)_selectedObjects.GetSize();

		_gizmo.SetTransform(Transform(avgPosition));
	}
}

void ToolSelect::_SetHoverObject(Entity* ho)
{
	_hoverObject = _owner.engine.pObjectTracker->Track(ho);

	_hoverObjectIsSelected = false;

	for (size_t i = 0; i < _selectedObjects.GetSize(); ++i)
		if (_hoverObject == _selectedObjects[i])
		{
			_hoverObjectIsSelected = true;
			break;
		}
}

void ToolSelect::Initialise()
{
	_gridSnap = .25f;
	_snapToWorld = false;
	_gizmoIsLocal = false;
}

void ToolSelect::Activate(UIContainer& properties, UIContainer& toolProperties)
{
	UIPropertyManipulator::AddPropertiesToContainer(Editor::PROPERTY_HEIGHT, _owner, _GetProperties(), this, toolProperties);
}

void ToolSelect::Cancel()
{
	_placing = false;
	_shouldCopy = false;
	ClearSelection();
}

void ToolSelect::MouseMove(const MouseData &mouseData)
{
	_hoverObjectIsSelected = false;

	if (mouseData.viewport >= 0 && _owner.GetVP(mouseData.viewport).camera.GetProjection().GetType() == EProjectionType::ORTHOGRAPHIC)
	{
		if (mouseData.isLeftDown)
		{
			if (_placing)
			{
				Vector2 p1, p2;
				EditorUtil::CalculatePointsFromMouseData(mouseData, p1, p2);

				Vector3 v;
				v[mouseData.rightElement] = p1[0];
				v[mouseData.upElement] = p1[1];
				v[mouseData.forwardElement] = _sbPoint1[mouseData.forwardElement];
				_sbPoint1 = v;

				v[mouseData.rightElement] = p2[0];
				v[mouseData.upElement] = p2[1];
				v[mouseData.forwardElement] = _sbPoint2[mouseData.forwardElement];
				_sbPoint2 = v;
			}
			else if (_hoverObject)
			{
				float deltaX = mouseData.unitX - mouseData.heldUnitX;
				float deltaY = mouseData.unitY - mouseData.heldUnitY;

				if (!_snapToWorld && _gridSnap)
				{
					deltaX = Maths::Round(deltaX, _gridSnap);
					deltaY = Maths::Round(deltaY, _gridSnap);
				}

				for (size_t i = 0; i < _selectedObjects.GetSize(); ++i)
				{
					Vector3 startPos = _selectedObjects[i]->GetWorldPosition();
					Vector3 endPos;
					endPos[mouseData.forwardElement] = startPos[mouseData.forwardElement];
					endPos[mouseData.rightElement] = _origObjectX + deltaX;
					endPos[mouseData.upElement] = _origObjectY + deltaY;

					if (_snapToWorld && _gridSnap)
					{
						endPos[mouseData.rightElement] = Maths::Round(endPos[mouseData.rightElement], _gridSnap);
						endPos[mouseData.upElement] = Maths::Round(endPos[mouseData.upElement], _gridSnap);
					}

					if (_shouldCopy && (endPos != startPos))
					{
						_shouldCopy = false;
						_CloneSelection();
					}

					_selectedObjects[i]->SetWorldPosition(endPos);
				}

				_owner.RefreshProperties();
			}
		}
		else if (!_placing)
		{
			Vector3 v;

			v[mouseData.rightElement] = (float)mouseData.unitX_rounded;
			v[mouseData.upElement] = (float)mouseData.unitY_rounded;
			v[mouseData.forwardElement] = -100.f;
			_sbPoint1 = v;
			
			v[mouseData.rightElement] = (float)mouseData.unitX_rounded + 1.f;
			v[mouseData.upElement] = (float)mouseData.unitY_rounded + 1.f;
			v[mouseData.forwardElement] = 100.f;
			_sbPoint2 = v;
			
			bool found = false;

			for (uint32 i = 0; i < _selectedObjects.GetSize(); ++i)
			{
				auto bounds = _selectedObjects[i]->GetWorldBounds();

				if (mouseData.unitX >= bounds.min[mouseData.rightElement] && mouseData.unitX <= bounds.max[mouseData.rightElement] && mouseData.unitY >= bounds.min[mouseData.upElement] && mouseData.unitY <= bounds.max[mouseData.upElement])
				{
					System::SetCursor(ECursor::HAND);
					_hoverObject = _selectedObjects[i];
					_hoverObjectIsSelected = true;
					found = true;
					break;
				}
			}

			if (found == false && _hoverObject)
				_hoverObject.Clear();
		}
	}

	if (!_placing && mouseData.viewport >= 0)
	{
		EntCamera& camera = _owner.GetVP(mouseData.viewport).camera;

		if (camera.GetProjection().GetType() == EProjectionType::PERSPECTIVE)
		{
			Ray r(camera.GetProjection().ScreenToWorld(camera.GetWorldTransform(), Vector2((float)mouseData.x / camera.GetProjection().GetDimensions().x, (float)mouseData.y / camera.GetProjection().GetDimensions().y)));

			//Gizmo
			{
				float t = INFINITY;
				_gizmo.Update(mouseData, r, t);

				if (t < INFINITY)
					System::SetCursor(ECursor::HAND);
			}

			_owner.RefreshProperties();

			//Update hoverobject
			Buffer<RaycastResult> results = _owner.LevelRef().Raycast(r);

			if (results.GetSize())
			{
				_SetHoverObject(results[0].object);

				if (_hoverObjectIsSelected == false)
					System::SetCursor(ECursor::HAND);
			}
			else
				_hoverObject.Clear();
		}
	}
}

void ToolSelect::MouseDown(const MouseData &mouseData)
{
	if (_gizmo.MouseDown())
	{
		if (_owner.engine.pInputManager->IsKeyDown(EKeycode::ALT))
			_shouldCopy = true;

		return;
	}

	if (mouseData.viewport >= 0)
	{
		if (_owner.GetVP(mouseData.viewport).camera.GetProjection().GetType() == EProjectionType::PERSPECTIVE)
		{
			_placing = false;

			if (!_hoverObjectIsSelected)
			{
				if (_hoverObject)
					Select(_hoverObject.Ptr());
				else if (!_owner.engine.pInputManager->IsKeyDown(EKeycode::LCTRL))
					ClearSelection();
			}
		}
		else if (_hoverObject)
		{
			if (_owner.engine.pInputManager->IsKeyDown(EKeycode::ALT))
				_shouldCopy = true;

			Vector3 objPos = _hoverObject->GetWorldPosition();

			_origObjectX = objPos[mouseData.rightElement];
			_origObjectY = objPos[mouseData.upElement];
		}
		else _placing = true;
	}
}

void ToolSelect::MouseUp(const MouseData& mouseData)
{
	_gizmo.MouseUp();
}

void ToolSelect::KeySubmit()
{
	_placing = false;
	ClearSelection();

	Buffer<Entity*> result = _owner.LevelRef().FindOverlappingChildren(Collider(ECollisionChannels::ALL, CollisionBox(Box::FromPoints(_sbPoint1, _sbPoint2))));

	_selectedObjects.SetSize(result.GetSize());

	for (size_t i = 0; i < _selectedObjects.GetSize(); ++i)
	{
		_selectedObjects[i] = _owner.engine.pObjectTracker->Track(result[i]);
		_selectedObjects[i]->onTransformChanged += Callback(this, &ToolSelect::_UpdateGizmoPos);
	}

	if (_selectedObjects.GetSize() > 0)
		_owner.ChangePropertyEntity(_selectedObjects.Last().Ptr());

	_UpdateGizmoPos();
}

void ToolSelect::KeyDelete()
{
	for (uint32 i = 0; i < _selectedObjects.GetSize(); ++i)
		_selectedObjects[i]->Delete(_owner.engine.context);

	ClearSelection();
}

void ToolSelect::Render(RenderQueue& q) const
{
	if (_placing)
	{
		RenderEntry& e = q.NewDynamicEntry(ERenderChannels::EDITOR);
		e.AddSetColour(Colour::Cyan);
		e.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
		e.AddBox(_sbPoint1, _sbPoint2);
	}
	
	if (_selectedObjects.GetSize() > 0 && EntCamera::Current()->GetProjection().GetType() == EProjectionType::PERSPECTIVE)
	{
		RenderEntry& pre = q.NewDynamicEntry(ERenderChannels::EDITOR);
		pre.AddCommand(RCMDSetDepthFunc::ALWAYS);
		_gizmo.Render(q);
		RenderEntry& post = q.NewDynamicEntry(ERenderChannels::EDITOR);
		post.AddCommand(RCMDSetDepthFunc::LEQUAL);
	}
	
	if (_selectedObjects.GetSize())
	{
		RenderEntry& e = q.NewDynamicEntry(ERenderChannels::EDITOR);
		e.AddSetColour(Colour::Cyan);
		e.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);

		for (uint32 i = 0; i < _selectedObjects.GetSize(); ++i)
		{
			Bounds bounds = _selectedObjects[i]->GetWorldBounds();

			e.AddBox(bounds.min, bounds.max);
		}
	}
}

void ToolSelect::Select(Entity* object)
{
	if (object == nullptr)
	{
		ClearSelection();
		return;
	}

	if (!_owner.engine.pInputManager->IsKeyDown(EKeycode::LCTRL))
		ClearSelection();

	_selectedObjects.Add(_owner.engine.pObjectTracker->Track(object));

	_owner.ChangePropertyEntity(_selectedObjects.Last().Ptr());
	_selectedObjects.Last()->onTransformChanged += Callback(this, &ToolSelect::_UpdateGizmoPos);
	_UpdateGizmoPos();
}

void ToolSelect::ClearSelection()
{
	for (size_t i = 0; i < _selectedObjects.GetSize(); ++i)
		if (_selectedObjects[i])
			_selectedObjects[i]->onTransformChanged -= Callback(this, &ToolSelect::_UpdateGizmoPos);

	_owner.ClearProperties();
	_selectedObjects.SetSize(0);
	_hoverObject.Clear();
}

void ToolSelect::_CloneSelection()
{
	for (size_t i = 0; i < _selectedObjects.GetSize(); ++i)
	{
		bool isHoverObject = _hoverObject == _selectedObjects[i];
		_selectedObjects[i]->onTransformChanged -= Callback(this, &ToolSelect::_UpdateGizmoPos);

		Entity* newObject = _selectedObjects[i]->Clone();
		newObject->GetProperties().Transfer(_selectedObjects[i].Ptr(), newObject, _owner.engine.context);

		_selectedObjects[i] = _owner.engine.pObjectTracker->Track(newObject);
		
		if (isHoverObject) _hoverObject = _selectedObjects[i];
		_selectedObjects[i]->onTransformChanged += Callback(this, &ToolSelect::_UpdateGizmoPos);
	}
	
	_hoverObjectIsSelected = true;
	_owner.ChangePropertyEntity(_selectedObjects.Last().Ptr());

	_UpdateGizmoPos();
}
