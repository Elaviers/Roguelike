#include "ToolSelect.h"
#include "Editor.h"
#include "EditorUtil.h"
#include <Engine/DrawUtils.h>
#include <Engine/RaycastResult.h>

void ToolSelect::Cancel()
{
	_placing = false;
	_selectedObjects.SetSize(0);
	_focusedObject = nullptr;
}

void ToolSelect::MouseMove(const MouseData &mouseData)
{
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
			else if (_focusedObject)
			{
				Vector3 newPos;
				newPos[mouseData.forwardElement] = _focusedObject->transform.Position()[mouseData.forwardElement];
				newPos[mouseData.rightElement] = mouseData.unitX + _dragOffsetX;
				newPos[mouseData.upElement] = mouseData.unitY + _dragOffsetY;

				_focusedObject->transform.SetPosition(newPos);
			}
		}
		else if (!_placing)
		{
			Vector3 v;

			v[mouseData.rightElement] = mouseData.unitX_rounded;
			v[mouseData.upElement] = mouseData.unitY_rounded;
			v[mouseData.forwardElement] = -100;
			_box.SetPoint1(v);

			v[mouseData.rightElement] = mouseData.unitX_rounded + 1;
			v[mouseData.upElement] = mouseData.unitY_rounded + 1;
			v[mouseData.forwardElement] = 100;
			_box.SetPoint2(v);

			GameObject *prevObj = _focusedObject;
			_focusedObject = nullptr;

			for (uint32 i = 0; i < _selectedObjects.GetSize(); ++i)
			{
				auto bounds = _selectedObjects[i]->GetWorldBounds();

				if (mouseData.unitX >= bounds.min[mouseData.rightElement] && mouseData.unitX <= bounds.max[mouseData.rightElement] && mouseData.unitY >= bounds.min[mouseData.upElement] && mouseData.unitY <= bounds.max[mouseData.upElement])
				{
					_focusedObject = _selectedObjects[i];
					::SetCursor(::LoadCursor(NULL, IDC_HAND));
					break;
				}
			}

			if (_focusedObject == nullptr && prevObj)
				::SetCursor(::LoadCursor(NULL, IDC_ARROW));
		}
	}
}

void ToolSelect::MouseDown(const MouseData &mouseData)
{
	if (_owner.CameraRef(mouseData.viewport).GetProjectionType() == ProjectionType::PERSPECTIVE)
	{
		_placing = false;

		ObjCamera &camera = _owner.CameraRef(mouseData.viewport);
		RECT windowDims;
		::GetClientRect(_owner.ViewportRef(mouseData.viewport).GetHwnd(), &windowDims);
		
		Ray r = camera.ScreenCoordsToRay(Vector2((float)mouseData.x / (float)windowDims.right, (float)mouseData.y / (float)windowDims.bottom));
		r.channel = RayChannel::CAMERA;

		Buffer<RaycastResult> results = _owner.LevelRef().Raycast(r);

		if (results.GetSize() > 0)
		{
			if (Engine::inputManager->IsKeyDown(Keycode::CTRL))
				_selectedObjects.Add(results[0].object);
			else
			{
				_selectedObjects.SetSize(1);
				_selectedObjects[0] = results[0].object;
				_owner.PropertyWindowRef().SetObject(_selectedObjects[0]);
			}
		}
		else if (!Engine::inputManager->IsKeyDown(Keycode::CTRL))
		{
			_selectedObjects.SetSize(0);
			_owner.PropertyWindowRef().Clear();
		}
	}
	else if (_focusedObject) 
	{
		_dragOffsetX = _focusedObject->transform.Position()[mouseData.rightElement] - mouseData.unitX;
		_dragOffsetY = _focusedObject->transform.Position()[mouseData.upElement] - mouseData.unitY;
	}
	else _placing = true;
}

void ToolSelect::KeySubmit()
{
	_placing = false;

	ColliderAABB aabb;
	aabb.min = _box.GetMin();
	aabb.max = _box.GetMax();

	_selectedObjects = _owner.LevelRef().FindOverlaps(aabb);
}

void ToolSelect::KeyDelete()
{
	for (uint32 i = 0; i < _selectedObjects.GetSize(); ++i)
		delete _selectedObjects[i];

	_owner.PropertyWindowRef().Clear();
	_selectedObjects.SetSize(0);
	_focusedObject = nullptr;
}

void ToolSelect::Render() const
{
	glLineWidth(3);
	glDepthFunc(GL_ALWAYS);

	if (_placing)
	{
		GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, Vector4(0.f, 1.f, 1.f, 1.f));
		_box.Render();
	}

	GLProgram::Current().SetVec4(DefaultUniformVars::vec4Colour, Vector4(1.f, 1.f, 0.f, 1.f));
	for (uint32 i = 0; i < _selectedObjects.GetSize(); ++i)
	{
		Bounds bounds = _selectedObjects[i]->GetWorldBounds();

		DrawUtils::DrawBox(*Engine::modelManager, bounds.min, bounds.max);
	}

	glDepthFunc(GL_LESS);
}
