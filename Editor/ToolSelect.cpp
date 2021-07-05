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
#include <ELPhys/CollisionBox.hpp>

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

	Vector3 savg;
	Vector3 eavg;
	for (size_t i = 0; i < _selection.GetSize(); ++i)
	{
		Vector3 startPos = _selection[i].object->GetAbsolutePosition();
		Vector3 endPos = startPos + d;

		if (!_gizmoIsLocal && _snapToWorld && _gridSnap)
		{
			endPos.x = Maths::Round(endPos.x, _gridSnap);
			endPos.y = Maths::Round(endPos.y, _gridSnap);
			endPos.z = Maths::Round(endPos.z, _gridSnap);
		}

		if (_shouldCopy && (endPos != startPos))
		{
			_shouldCopy = false;
			_CloneSelection();
		}

		_selection[i].object->SetAbsolutePosition(endPos);

		savg += startPos;
		eavg += endPos;
	}

	savg /= _selection.GetSize();
	eavg /= _selection.GetSize();
	return eavg - savg;
}

float ToolSelect::_GizmoRotate(const Vector3 &axis, float angle)
{
	Rotation rotation(Quaternion(axis, angle));

	if (rotation.GetEuler().LengthSquared() > 0.f && _shouldCopy)
	{
		_shouldCopy = false;
		_CloneSelection();
	}

	for (size_t i = 0; i < _selection.GetSize(); ++i)
	{
		_selection[i].object->SetAbsolutePosition(
			VectorMaths::RotateAbout(_selection[i].object->GetAbsolutePosition(), _gizmo.GetTransform().GetPosition(), rotation));

		_selection[i].object->SetAbsoluteRotation(_selection[i].object->GetAbsoluteRotation() * rotation);
	}

	return _gizmoIsLocal ? 0.f : angle;
}

void ToolSelect::_GizmoSetSidePos(E2DBrushEdge edge, const Vector3& delta)
{
	//note: will not work for any plane that is not axis-aligned!

	WorldObject* ent = _selection[0].object.Ptr();
	Transform wt = ent->GetAbsoluteTransform();
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
			finalEdgePos[elem] = Maths::Round(finalEdgePos[elem], _gridSnap);
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
	ent->SetAbsoluteTransform(wt);
}

void ToolSelect::_UpdateGizmoTransform()
{
	if (_gizmoIsLocal && _selection.GetSize() == 1)
		_gizmo.SetPositionSize(Transform(_selection[0].object->GetAbsolutePosition(), _selection[0].object->GetAbsoluteRotation()));
	else
	{
		Vector3 avgPosition;

		for (size_t i = 0; i < _selection.GetSize(); ++i)
			avgPosition += _selection[i].object->GetAbsolutePosition();

		avgPosition /= (float)_selection.GetSize();

		_gizmo.SetPositionSize(Transform(avgPosition));

		if (_selection.GetSize())
			_brushGizmo.SetObjectTransform(_selection[0].object->GetAbsoluteTransform());
	}
}

void ToolSelect::_SetHoverObject(WorldObject* ho)
{
	if (ho != _hoverObject.Ptr())
	{
		_hoverObject = _owner.WorldRef().TrackObject(ho);

		_hoverObjectIsSelected = false;

		for (size_t i = 0; i < _selection.GetSize(); ++i)
			if (_hoverObject == _selection[i].object)
			{
				_hoverObjectIsSelected = true;
				break;
			}
	}
}

bool ToolSelect::_ViewportCanRaySelect(const Viewport& vp) const
{
	return vp.GetCameraType() == Viewport::ECameraType::PERSPECTIVE || vp.GetCameraType() == Viewport::ECameraType::ISOMETRIC;
}

bool ToolSelect::_ViewportIsOrtho(const Viewport& vp) const
{
	return vp.GetCameraType() != Viewport::ECameraType::PERSPECTIVE;
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

	_gizmo.SetMoveFunction(Function(&ToolSelect::_GizmoMove, *this));
	_gizmo.SetRotateFunction(Function(&ToolSelect::_GizmoRotate, *this));

	_brushGizmo.SetSideFunction(Function(&ToolSelect::_GizmoSetSidePos, *this));
}

void ToolSelect::Activate(UIContainer& properties, UIContainer& toolProperties)
{
	UIPropertyManipulator::AddPropertiesToContainer(1.f, Editor::PROPERTY_HEIGHT, _owner, _GetProperties(), this, toolProperties);
}

void ToolSelect::Cancel()
{
	_placing = false;
	_shouldCopy = false;
	ClearSelection();
}

void ToolSelect::MouseMove(MouseData &mouseData)
{
	_hoverObjectIsSelected = false;

	Viewport& vp = *mouseData.viewport;
	Ray r(vp.cameraProjection.ScreenToWorld(vp.cameraTransform, Vector2((float)mouseData.x / vp.cameraProjection.GetDimensions().x, (float)mouseData.y / vp.cameraProjection.GetDimensions().y)));

	if (_activeGizmo) {
		float t = INFINITY;
		_activeGizmo->Update(mouseData, r, t);

		if (_activeGizmo->IsBeingDragged()) {
			_owner.RefreshProperties();
			return;
		}

		if (t < INFINITY)
		{
			_hoverObject.Clear();
			_owner.TrySetCursor(ECursor::HAND);
			return;
		}
	}

	if (mouseData.viewport && _ViewportIsOrtho(*mouseData.viewport))
	{
		EAxis fwdAxis = mouseData.viewport->gridAxis;
		int fwdElement = (int)fwdAxis;
		int rightElement = (int)Axes::GetHorizontalAxis(fwdAxis);
		int upElement = (int)Axes::GetVerticalAxis(fwdAxis);

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
			else if (_dragging)
			{
				float deltaX = mouseData.unitX - mouseData.heldUnitX;
				float deltaY = mouseData.unitY - mouseData.heldUnitY;

				if (!_snapToWorld && _gridSnap)
				{
					deltaX = Maths::Round(deltaX, _gridSnap);
					deltaY = Maths::Round(deltaY, _gridSnap);
				}

				for (size_t i = 0; i < _selection.GetSize(); ++i)
				{
					Vector3 startPos = _selection[i].object->GetAbsolutePosition();
					Vector3 endPos;
					endPos[fwdElement] = startPos[fwdElement];
					endPos[rightElement] = _selection[i].dragStartX + deltaX;
					endPos[upElement] = _selection[i].dragStartY + deltaY;

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

					_selection[i].object->SetAbsolutePosition(endPos);
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

			for (uint32 i = 0; i < _selection.GetSize(); ++i)
			{
				const Volume* v = _selection[i].object->GetVolume();

				if (v)
				{
					Vector3 min, max;
					v->Bounds_AABB(_selection[i].object->GetAbsoluteTransform(), min, max);

					if (mouseData.unitX >= min[rightElement] && 
						mouseData.unitX <= max[rightElement] && 
						mouseData.unitY >= min[upElement] && 
						mouseData.unitY <= max[upElement])
					{
						_owner.TrySetCursor(ECursor::HAND);
						_hoverObject = _selection[i].object;
						_hoverObjectIsSelected = true;
						found = true;
						break;
					}
				}
			}

			if (found == false && _hoverObject)
				_hoverObject.Clear();
		}
	}

	if (!_placing)
	{
		if (mouseData.hoverObject)
		{
			_SetHoverObject(mouseData.hoverObject);

			if (_hoverObjectIsSelected == false)
				_owner.TrySetCursor(ECursor::HAND);
		}
		else
			_hoverObject.Clear();
	}
}

void ToolSelect::MouseDown(MouseData &mouseData)
{
	if (_activeGizmo && _activeGizmo->MouseDown())
	{
		if (_owner.engine.pInputManager->IsKeyDown(EKeycode::LALT))
			_shouldCopy = true;

		return;
	}

	if (mouseData.viewport)
	{
		bool ortho = _ViewportIsOrtho(*mouseData.viewport);
		if (ortho)
		{
			if (_hoverObject && _hoverObjectIsSelected)
			{
				if (_owner.engine.pInputManager->IsKeyDown(EKeycode::LALT))
					_shouldCopy = true;

				int xElem = (int)Axes::GetHorizontalAxis(mouseData.viewport->gridAxis);
				int yElem = (int)Axes::GetVerticalAxis(mouseData.viewport->gridAxis);

				for (size_t i = 0; i < _selection.GetSize(); ++i)
				{
					Vector3 objPos = _selection[i].object->GetAbsolutePosition();
					_selection[i].dragStartX = objPos[xElem];
					_selection[i].dragStartY = objPos[yElem];
				}

				_placing = false;
				_dragging = true;
				return;
			}
		}

		if (_ViewportCanRaySelect(*mouseData.viewport))
		{
			_placing = false;

			if (!_hoverObjectIsSelected)
				Select(_hoverObject.Ptr());
		}
		else if (ortho) _placing = true;
	}
}

void ToolSelect::MouseUp(MouseData& mouseData)
{
	if (_activeGizmo) _activeGizmo->MouseUp();

	_dragging = false;
}

void ToolSelect::KeySubmit()
{
	_placing = false;
	ClearSelection();

	Buffer<WorldObject*> result = _owner.WorldRef().FindOverlaps(
		Collider(ECollisionChannels::ALL, CollisionBox(Box::FromPoints(_sbPoint1, _sbPoint2))),
		Transform::Identity());

	_selection.SetSize(result.GetSize());

	for (size_t i = 0; i < _selection.GetSize(); ++i)
	{
		_selection[i].object = _owner.WorldRef().TrackObject(result[i]);
	}

	if (_selection.GetSize() > 0)
		_owner.ChangePropertyWorldObject(_selection[_selection.GetSize() - 1].object.Ptr());
}

void ToolSelect::KeyDelete()
{
	for (uint32 i = 0; i < _selection.GetSize(); ++i)
		_selection[i].object->Destroy();

	ClearSelection();
}

void ToolSelect::Update(float deltaSeconds)
{
	//Get rid of any deleted entities
	bool pruned = false;
	for (size_t i = 0; i < _selection.GetSize();)
	{
		if (!_selection[i].object)
		{
			pruned = true;
			_selection.Remove(i);
		}
		else ++i;
	}

	if (pruned)
		_OnSelectionChanged();

	_UpdateGizmoTransform();
}

void ToolSelect::Render(RenderQueue& q) const
{
	if (_placing)
	{
		RenderEntry& e = q.CreateEntry(ERenderChannels::EDITOR);
		e.AddSetLineWidth(2.f);
		e.AddSetColour(Colour::Cyan);
		e.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);
		e.AddBox(_sbPoint1, _sbPoint2);
	}
	
	if (_selection.GetSize())
	{
		RenderEntry& e = q.CreateEntry(ERenderChannels::EDITOR);
		e.AddSetLineWidth(2.f);
		e.AddSetColour(Colour::Cyan);
		e.AddSetTexture(RCMDSetTexture::Type::WHITE, 0);

		for (uint32 i = 0; i < _selection.GetSize(); ++i)
		{
			const Volume* v = _selection[i].object->GetVolume();
			if (v)
			{
				Vector3 min, max;
				v->Bounds_AABB(_selection[i].object->GetAbsoluteTransform(), min, max);
				e.AddBox(min, max);
			}
		}
	}

	if (_activeGizmo)
	{
		q.CreateEntry(ERenderChannels::EDITOR).AddCommand(RCMDSetDepthFunc::ALWAYS);
		_activeGizmo->Render(q);
		q.CreateEntry(ERenderChannels::EDITOR).AddCommand(RCMDSetDepthFunc::LEQUAL);
	}
}

void ToolSelect::Select(WorldObject* object)
{
	if (!_owner.engine.pInputManager->IsKeyDown(EKeycode::LCTRL))
		ClearSelection();

	if (object == nullptr)
		return;

	_selection.Add(WorldObjectSelection{_owner.WorldRef().TrackObject(object)});

	_OnSelectionChanged();
}

void ToolSelect::ClearSelection()
{
	_owner.ClearProperties();
	_selection.Clear();
	_hoverObject.Clear();

	_activeGizmo = nullptr;
}

void ToolSelect::GetSelection(Buffer<WorldObject*>& selection) const
{
	for (auto& item : _selection)
		selection.Add(item.object.Ptr());
}

void ToolSelect::SetSelection(const Buffer<WorldObject*>& selection)
{
	ClearSelection();
	_selection.SetSize(selection.GetSize());

	for (size_t i = 0; i < _selection.GetSize(); ++i)
		_selection[i].object = _owner.WorldRef().TrackObject(selection[i]);

	_OnSelectionChanged();
}

void ToolSelect::_OnSelectionChanged()
{
	OGeometryCollection* g;
	if (_selection.GetSize() == 1 && (g = dynamic_cast<OGeometryCollection*>(_selection[0].object.Ptr())))
	{
		ClearSelection();
		_owner.tools.iso.SetTarget(g);
		_owner.SetTool(ETool::ISO);
		return;
	}

	int brush2ds = 0;
	for (auto& item : _selection)
	{
		if (dynamic_cast<OBrush2D*>(item.object.Ptr())) ++brush2ds;
	}

	_activeGizmo = nullptr;
	if (_selection.GetSize() > 0)
	{
		if (brush2ds > 0 && brush2ds == _selection.GetSize())
			_activeGizmo = &_brushGizmo;
		else
			_activeGizmo = &_gizmo;

		_owner.ChangePropertyWorldObject(_selection[_selection.GetSize() - 1].object.Ptr());
	}
	else
		_owner.ClearProperties();
}

void ToolSelect::_CloneSelection()
{
	for (size_t i = 0; i < _selection.GetSize(); ++i)
	{
		bool isHoverObject = _hoverObject == _selection[i].object;

		WorldObject* newObject = _owner.WorldRef().CloneObject(*_selection[i].object, false);
		newObject->GetProperties().Transfer(_selection[i].object.Ptr(), newObject, _owner.engine.context);

		_selection[i].object = _owner.WorldRef().TrackObject(newObject);
		
		if (isHoverObject) _hoverObject = _selection[i].object;
	}
	
	_hoverObjectIsSelected = true;
	_owner.ChangePropertyWorldObject(_selection[_selection.GetSize() - 1].object.Ptr());
}
