#include "ToolSelect.hpp"
#include "Editor.hpp"
#include "EditorUtil.hpp"
#include "GizmoAxis.hpp"
#include "GizmoBrushSizer2D.hpp"
#include "GizmoPlane.hpp"
#include "GizmoRing.hpp"
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

	properties.Add(
		"GizmoIsLocal",
		MemberGetter<ToolSelect, bool>(&ToolSelect::_GetGizmoIsLocal),
		MemberSetter<ToolSelect, bool>(&ToolSelect::_SetGizmoIsLocal));
	DO_ONCE_END;

	return properties;
}

Vector3 ToolSelect::_GizmoMove(const Vector3& delta)
{
	//todo: gizmo snapping for local mode

	Vector3 d(delta);

	if (!_gizmoIsLocal && !_snapToWorld && _gridSnap)
	{
		d.x = Maths::Trunc(d.x, _gridSnap);
		d.y = Maths::Trunc(d.y, _gridSnap);
		d.z = Maths::Trunc(d.z, _gridSnap);
	}

	for (size_t i = 0; i < _selectedObjects.GetSize(); ++i)
	{
		Vector3 startPos = _selectedObjects[i]->GetWorldPosition();
		Vector3 endPos = startPos + d;

		if (!_gizmoIsLocal && _snapToWorld && _gridSnap)
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

	return d;
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
			VectorMaths::RotateAbout(_selectedObjects[i]->GetWorldPosition(), _gizmo.GetTransform().GetPosition(), rotation));

		_selectedObjects[i]->SetWorldRotation(_selectedObjects[i]->GetWorldRotation() * rotation);
	}

	return _gizmoIsLocal ? 0.f : angle;
}

void ToolSelect::_GizmoSetSidePos(E2DBrushEdge edge, const Vector3& delta)
{
	//note: will not work for any plane that is not axis-aligned!

	Entity* ent = _selectedObjects[0].Ptr();
	Transform wt = ent->GetWorldTransform();
	Vector3 scale = wt.GetScale();

	Vector3 normal;
	int elem = -1;
	switch (edge)
	{
	case E2DBrushEdge::NEG_X:
		normal = -wt.GetRightVector();
		elem = 0;
		break;
	case E2DBrushEdge::POS_X:
		normal = wt.GetRightVector();
		elem = 0;
		break;
	case E2DBrushEdge::NEG_Y:
		normal = -wt.GetForwardVector();
		elem = 2;
		break;
	case E2DBrushEdge::POS_Y:
		normal = wt.GetForwardVector();
		elem = 2;
		break;

	default:
		Debug::Break(); //This should never execute!
		return;
	}

	Vector3 anchor = wt.GetPosition() - normal * (scale[elem] / 2.f);
	Vector3 edgePos = anchor + normal * scale[elem];
	Vector3 finalEdgePos;
	if (_gridSnap)
	{
		if (_snapToWorld)
		{
			finalEdgePos = edgePos + delta;
			finalEdgePos[elem] = Maths::Trunc(finalEdgePos[elem], _gridSnap);
		}
		else
		{
			Vector3 d = delta;
			d[elem] = Maths::Trunc(d[elem], _gridSnap);
			finalEdgePos = edgePos + d;
		}
	}
	else finalEdgePos = edgePos + delta;

	wt.SetPosition((anchor + finalEdgePos) / 2.f);
	scale[elem] = (anchor - finalEdgePos).Length();
	wt.SetScale(scale);
	ent->SetWorldTransform(wt);
}

void ToolSelect::_UpdateGizmoTransform()
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
		_brushGizmo.SetObjectTransform(_selectedObjects[0]->GetWorldTransform());
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

bool ToolSelect::_ViewportCanRaySelect(const Viewport& vp) const
{
	return vp.GetCameraType() == Viewport::ECameraType::PERSPECTIVE || vp.GetCameraType() == Viewport::ECameraType::ISOMETRIC;
}

void ToolSelect::Initialise()
{
	_gridSnap = .25f;
	_snapToWorld = false;
	_gizmoIsLocal = false;

	_gizmo.AddComponent(GizmoAxis(Colour::Red, Vector3(1, 0, 0)));
	_gizmo.AddComponent(GizmoAxis(Colour::Green, Vector3(0, 1, 0)));
	_gizmo.AddComponent(GizmoAxis(Colour::Blue, Vector3(0, 0, 1)));
	_gizmo.AddComponent(GizmoRing(Colour::Red, Vector3(0, 90, 0)));
	_gizmo.AddComponent(GizmoRing(Colour::Green, Vector3(90, 0, 0)));
	_gizmo.AddComponent(GizmoRing(Colour::Blue, Rotation()));
	_gizmo.AddComponent(GizmoPlane(Colour(1.f, 1.f, 0.f), Transform(Vector3(0.5f, 0.5f, 0.f), Rotation(), Vector3(0.25f, 0.25f, 0.25f))));
	_gizmo.AddComponent(GizmoPlane(Colour(1.f, 0.f, 1.f), Transform(Vector3(0.5f, 0.f, 0.5f), Vector3(-90.f, 0.f, 0.f), Vector3(0.25f, 0.25f, 0.25f))));
	_gizmo.AddComponent(GizmoPlane(Colour(0.f, 1.f, 1.f), Transform(Vector3(0.0f, 0.5f, 0.5f), Vector3(0.f, -90.f, 0.f), Vector3(0.25f, 0.25f, 0.25f))));

	_brushGizmo.AddComponent(GizmoBrushSizer2D(Colour::Yellow, E2DBrushEdge::NEG_X));
	_brushGizmo.AddComponent(GizmoBrushSizer2D(Colour::Yellow, E2DBrushEdge::POS_X));
	_brushGizmo.AddComponent(GizmoBrushSizer2D(Colour::Yellow, E2DBrushEdge::NEG_Y));
	_brushGizmo.AddComponent(GizmoBrushSizer2D(Colour::Yellow, E2DBrushEdge::POS_Y));

	_gizmo.SetMoveFunction(FunctionPointer<Vector3, const Vector3&>(this, &ToolSelect::_GizmoMove));
	_gizmo.SetRotateFunction(FunctionPointer<float, const Vector3&, float>(this, &ToolSelect::_GizmoRotate));

	_brushGizmo.SetSideFunction(FunctionPointer<void, E2DBrushEdge, const Vector3&>(this, &ToolSelect::_GizmoSetSidePos));
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

	bool canRaySelect = _ViewportCanRaySelect(*mouseData.viewport);

	if (!canRaySelect)
	{
		Axes::EAxis fwdAxis = mouseData.viewport->gridAxis;
		int fwdElement = fwdAxis;
		int rightElement = Axes::GetHorizontalAxis(fwdAxis);
		int upElement = Axes::GetVerticalAxis(fwdAxis);

		if (mouseData.isLeftDown)
		{
			if (_placing)
			{
				Vector2 p1, p2;
				EditorUtil::CalculatePointsFromMouseData(mouseData, p1, p2);

				Vector3 v;
				v[rightElement] = p1[0];
				v[upElement] = p1[1];
				v[fwdElement] = _sbPoint1[fwdElement];
				_sbPoint1 = v;

				v[rightElement] = p2[0];
				v[upElement] = p2[1];
				v[fwdElement] = _sbPoint2[fwdElement];
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
					endPos[fwdElement] = startPos[fwdElement];
					endPos[rightElement] = _origObjectX + deltaX;
					endPos[upElement] = _origObjectY + deltaY;

					if (_snapToWorld && _gridSnap)
					{
						endPos[rightElement] = Maths::Round(endPos[rightElement], _gridSnap);
						endPos[upElement] = Maths::Round(endPos[upElement], _gridSnap);
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

			v[rightElement] = (float)mouseData.unitX_rounded;
			v[upElement] = (float)mouseData.unitY_rounded;
			v[fwdElement] = -100.f;
			_sbPoint1 = v;
			
			v[rightElement] = (float)mouseData.unitX_rounded + 1.f;
			v[upElement] = (float)mouseData.unitY_rounded + 1.f;
			v[fwdElement] = 100.f;
			_sbPoint2 = v;
			
			bool found = false;

			for (uint32 i = 0; i < _selectedObjects.GetSize(); ++i)
			{
				auto bounds = _selectedObjects[i]->GetWorldBounds();

				if (mouseData.unitX >= bounds.min[rightElement] && mouseData.unitX <= bounds.max[rightElement] && mouseData.unitY >= bounds.min[upElement] && mouseData.unitY <= bounds.max[upElement])
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

	if (!_placing)
	{
		EntCamera& camera = mouseData.viewport->camera;

		Ray r(camera.GetProjection().ScreenToWorld(camera.GetWorldTransform(), Vector2((float)mouseData.x / camera.GetProjection().GetDimensions().x, (float)mouseData.y / camera.GetProjection().GetDimensions().y)));

		if (_activeGizmo) {
			float t = INFINITY;
			_activeGizmo->Update(mouseData, r, t);

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

void ToolSelect::MouseDown(const MouseData &mouseData)
{
	if (_activeGizmo && _activeGizmo->MouseDown())
	{
		if (_owner.engine.pInputManager->IsKeyDown(EKeycode::ALT))
			_shouldCopy = true;

		return;
	}

	if (mouseData.viewport)
	{
		if (_ViewportCanRaySelect(*mouseData.viewport))
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

			_origObjectX = objPos[Axes::GetHorizontalAxis(mouseData.viewport->gridAxis)];
			_origObjectY = objPos[Axes::GetVerticalAxis(mouseData.viewport->gridAxis)];
		}
		else _placing = true;
	}
}

void ToolSelect::MouseUp(const MouseData& mouseData)
{
	if (_activeGizmo) _activeGizmo->MouseUp();
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
		_selectedObjects[i]->onTransformChanged += Callback(this, &ToolSelect::_UpdateGizmoTransform);
	}

	if (_selectedObjects.GetSize() > 0)
		_owner.ChangePropertyEntity(_selectedObjects.Last().Ptr());

	_UpdateGizmoTransform();
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
	
	if (_activeGizmo)
	{
		RenderEntry& pre = q.NewDynamicEntry(ERenderChannels::EDITOR);
		pre.AddCommand(RCMDSetDepthFunc::ALWAYS);
		_activeGizmo->Render(q);
		RenderEntry& post = q.NewDynamicEntry(ERenderChannels::EDITOR);
		post.AddCommand(RCMDSetDepthFunc::LEQUAL);
	}
	
	if (_activeGizmo != &_brushGizmo && _selectedObjects.GetSize())
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
	_selectedObjects.Last()->onTransformChanged += Callback(this, &ToolSelect::_UpdateGizmoTransform);
	_UpdateGizmoTransform();

	_activeGizmo = dynamic_cast<EntBrush2D*>(_selectedObjects[0].Ptr()) ? &_brushGizmo : &_gizmo;
}

void ToolSelect::ClearSelection()
{
	for (size_t i = 0; i < _selectedObjects.GetSize(); ++i)
		if (_selectedObjects[i])
			_selectedObjects[i]->onTransformChanged -= Callback(this, &ToolSelect::_UpdateGizmoTransform);

	_owner.ClearProperties();
	_selectedObjects.SetSize(0);
	_hoverObject.Clear();

	_activeGizmo = nullptr;
}

void ToolSelect::_CloneSelection()
{
	for (size_t i = 0; i < _selectedObjects.GetSize(); ++i)
	{
		bool isHoverObject = _hoverObject == _selectedObjects[i];
		_selectedObjects[i]->onTransformChanged -= Callback(this, &ToolSelect::_UpdateGizmoTransform);

		Entity* newObject = _selectedObjects[i]->Clone();
		newObject->GetProperties().Transfer(_selectedObjects[i].Ptr(), newObject, _owner.engine.context);

		_selectedObjects[i] = _owner.engine.pObjectTracker->Track(newObject);
		
		if (isHoverObject) _hoverObject = _selectedObjects[i];
		_selectedObjects[i]->onTransformChanged += Callback(this, &ToolSelect::_UpdateGizmoTransform);
	}
	
	_hoverObjectIsSelected = true;
	_owner.ChangePropertyEntity(_selectedObjects.Last().Ptr());

	_UpdateGizmoTransform();
}
