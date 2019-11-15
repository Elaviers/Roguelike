#include "ToolSelect.hpp"
#include "Editor.hpp"
#include "EditorUtil.hpp"
#include <Engine/DrawUtils.hpp>
#include <Engine/InputManager.hpp>
#include <Engine/MacroUtilities.hpp>
#include <Engine/RaycastResult.hpp>

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
		d[0] = Maths::Trunc(d[0], _gridSnap);
		d[1] = Maths::Trunc(d[1], _gridSnap);
		d[2] = Maths::Trunc(d[2], _gridSnap);
	}

	for (size_t i = 0; i < _selectedObjects.GetSize(); ++i)
	{
		Vector3 pos = _selectedObjects[i]->GetWorldPosition() + d;

		if (_snapToWorld && _gridSnap)
		{
			pos[0] = Maths::Trunc(pos[0], _gridSnap);
			pos[1] = Maths::Trunc(pos[1], _gridSnap);
			pos[2] = Maths::Trunc(pos[2], _gridSnap);
		}

		_selectedObjects[i]->SetWorldPosition(pos);
	}
}

float ToolSelect::_GizmoRotate(const Vector3 &axis, float angle)
{
	Rotation rotation(Quaternion(axis, angle));

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
	_hoverObject = Engine::Instance().pObjectTracker->Track(ho);

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

void ToolSelect::Activate(PropertyWindow& properties, PropertyWindow& toolProperties)
{
	toolProperties.SetCvars(_GetProperties(), this);
	_owner.PropertyWindowRef().Clear();
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

	if (_owner.CameraRef(mouseData.viewport).GetProjectionType() == ProjectionType::ORTHOGRAPHIC)
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
				v[mouseData.forwardElement] = _box.GetPoint1()[mouseData.forwardElement];
				_box.SetPoint1(v);

				v[mouseData.rightElement] = p2[0];
				v[mouseData.upElement] = p2[1];
				v[mouseData.forwardElement] = _box.GetPoint2()[mouseData.forwardElement];
				_box.SetPoint2(v);
			}
			else if (_hoverObject)
			{
				if (_shouldCopy)
				{
					_shouldCopy = false;
					Entity *newObject = _hoverObject->Clone();
					
					_GetProperties().Transfer(_hoverObject.Ptr(), newObject);

					_hoverObject = Engine::Instance().pObjectTracker->Track(newObject);
					_hoverObjectIsSelected = true;
					_selectedObjects.SetSize(1);
					_selectedObjects[0] = _hoverObject;
					_owner.PropertyWindowRef().SetObject(_selectedObjects[0].Ptr());
				}

				float deltaX = mouseData.unitX - mouseData.heldUnitX;
				float deltaY = mouseData.unitY - mouseData.heldUnitY;

				if (!_snapToWorld && _gridSnap)
				{
					deltaX = Maths::Trunc(deltaX, _gridSnap);
					deltaY = Maths::Trunc(deltaY, _gridSnap);
				}

				Vector3 newPos;
				newPos[mouseData.forwardElement] = _hoverObject->GetWorldPosition()[mouseData.forwardElement];
				newPos[mouseData.rightElement] = _origObjectX + deltaX;
				newPos[mouseData.upElement] = _origObjectY + deltaY;

				if (_snapToWorld && _gridSnap)
				{
					newPos[mouseData.rightElement] =	Maths::Trunc(newPos[mouseData.rightElement],	_gridSnap);
					newPos[mouseData.upElement] =		Maths::Trunc(newPos[mouseData.upElement],		_gridSnap);
				}

				_hoverObject->SetWorldPosition(newPos);
			}
		}
		else if (!_placing)
		{
			Vector3 v;

			v[mouseData.rightElement] = (float)mouseData.unitX_rounded;
			v[mouseData.upElement] = (float)mouseData.unitY_rounded;
			v[mouseData.forwardElement] = -100.f;
			_box.SetPoint1(v);

			v[mouseData.rightElement] = (float)mouseData.unitX_rounded + 1.f;
			v[mouseData.upElement] = (float)mouseData.unitY_rounded + 1.f;
			v[mouseData.forwardElement] = 100.f;
			_box.SetPoint2(v);
			
			bool found = false;

			for (uint32 i = 0; i < _selectedObjects.GetSize(); ++i)
			{
				auto bounds = _selectedObjects[i]->GetWorldBounds();

				if (mouseData.unitX >= bounds.min[mouseData.rightElement] && mouseData.unitX <= bounds.max[mouseData.rightElement] && mouseData.unitY >= bounds.min[mouseData.upElement] && mouseData.unitY <= bounds.max[mouseData.upElement])
				{
					::SetCursor(::LoadCursor(NULL, IDC_HAND));
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
		EntCamera& camera = _owner.CameraRef(mouseData.viewport);

		if (camera.GetProjectionType() == ProjectionType::PERSPECTIVE)
		{
			RECT windowDims;
			::GetClientRect(_owner.ViewportRef(mouseData.viewport).GetHwnd(), &windowDims);

			Ray r(camera.ScreenCoordsToRay(Vector2((float)mouseData.x / (float)windowDims.right, (float)mouseData.y / (float)windowDims.bottom)));
			r.channels = COLL_ALL_CHANNELS;

			//Gizmo
			if (_selectedObjects.GetSize())
			{
				float t = INFINITY;
				_gizmo.Update(mouseData, r, t);

				if (t < INFINITY)
					::SetCursor(::LoadCursor(NULL, IDC_HAND));
			}

			//Update hoverobject
			Buffer<RaycastResult> results = _owner.LevelRef().Raycast(r);

			if (results.GetSize())
			{
				_SetHoverObject(results[0].object);

				if (_hoverObjectIsSelected == false)
					::SetCursor(::LoadCursor(NULL, IDC_HAND));
			}
			else
				_hoverObject.Clear();
		}
	}
}

void ToolSelect::MouseDown(const MouseData &mouseData)
{
	if (_gizmo.MouseDown()) return;

	if (_owner.CameraRef(mouseData.viewport).GetProjectionType() == ProjectionType::PERSPECTIVE)
	{
		_placing = false;

		if (_hoverObject && !_hoverObjectIsSelected)
			Select(_hoverObject.Ptr());
		else if (!Engine::Instance().pInputManager->IsKeyDown(Keycode::CTRL))
			ClearSelection();
	}
	else if (_hoverObject) 
	{
		if (Engine::Instance().pInputManager->IsKeyDown(Keycode::ALT))
			_shouldCopy = true;

		Vector3 objPos = _hoverObject->GetWorldPosition();

		_origObjectX = objPos[mouseData.rightElement];
		_origObjectY = objPos[mouseData.upElement];
	}
	else _placing = true;
}

void ToolSelect::MouseUp(const MouseData& mouseData)
{
	_gizmo.MouseUp();
}

void ToolSelect::KeySubmit()
{
	_placing = false;
	ClearSelection();

	Buffer<Entity*> result = _owner.LevelRef().FindOverlaps(ColliderBox(COLL_ALL_CHANNELS, Box::FromMinMax(_box.GetMin(), _box.GetMax())));

	_selectedObjects.SetSize(result.GetSize());

	for (size_t i = 0; i < _selectedObjects.GetSize(); ++i)
		_selectedObjects[i] = Engine::Instance().pObjectTracker->Track(result[i]);
}

void ToolSelect::KeyDelete()
{
	for (uint32 i = 0; i < _selectedObjects.GetSize(); ++i)
		_selectedObjects[i]->Delete();

	ClearSelection();
}

void ToolSelect::Render(EnumRenderChannel channels) const
{
	glLineWidth(3);
	
	if (_placing)
	{
		glDepthFunc(GL_ALWAYS);
		GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, Colour::Cyan);
		_box.Render(channels);
		glDepthFunc(GL_LESS);
	}
	
	if (_selectedObjects.GetSize() > 0 && EntCamera::Current()->GetProjectionType() == ProjectionType::PERSPECTIVE)
	{
		glDepthFunc(GL_ALWAYS);
		_gizmo.Draw();
		glDepthFunc(GL_LESS);
	}
	

	GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, Colour::Yellow);
	for (uint32 i = 0; i < _selectedObjects.GetSize(); ++i)
	{
		Bounds bounds = _selectedObjects[i]->GetWorldBounds();

		DrawUtils::DrawBox(*Engine::Instance().pModelManager, bounds.min, bounds.max);
	}
}

void ToolSelect::Select(Entity* object)
{
	if (Engine::Instance().pInputManager->IsKeyDown(Keycode::CTRL))
		_selectedObjects.Add(Engine::Instance().pObjectTracker->Track(object));
	else
	{
		_selectedObjects.SetSize(1);
		_selectedObjects[0] = Engine::Instance().pObjectTracker->Track(object);
		_owner.PropertyWindowRef().SetObject(_selectedObjects[0].Ptr());
	}
	
	_selectedObjects.Last()->onTransformChanged += Callback(this, &ToolSelect::_UpdateGizmoPos);
	_UpdateGizmoPos();
}

void ToolSelect::ClearSelection()
{
	for (size_t i = 0; i < _selectedObjects.GetSize(); ++i)
		if (_selectedObjects[i])
			_selectedObjects[i]->onTransformChanged -= Callback(this, &ToolSelect::_UpdateGizmoPos);

	_owner.PropertyWindowRef().Clear();
	_selectedObjects.SetSize(0);
	_hoverObject.Clear();
}
