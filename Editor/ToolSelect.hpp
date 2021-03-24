#pragma once
#include "Tool.hpp"
#include "Gizmo.hpp"
#include <Engine/WorldObject.hpp>
#include <ELCore/Buffer.hpp>
#include <ELCore/SharedPointer.hpp>

class Viewport;

class ToolSelect : public Tool
{
	bool _dragging;
	bool _placing;
	bool _shouldCopy;

	SharedPointer<WorldObject> _hoverObject;
	bool _hoverObjectIsSelected;

	struct WorldObjectSelection
	{
		SharedPointer<WorldObject> object;
		float dragStartX;
		float dragStartY;
	};

	Buffer<WorldObjectSelection> _selection;

	float _gridSnap;
	bool _snapToWorld;
	bool _gizmoIsLocal;

	//property callbacks
	bool _GetGizmoIsLocal() const { return _gizmoIsLocal; }
	void _SetGizmoIsLocal(const bool &gizmoIsLocal) { _gizmoIsLocal = gizmoIsLocal; _UpdateGizmoTransform(); }

	Gizmo _gizmo;
	Gizmo _brushGizmo;

	Gizmo* _activeGizmo;

	Vector3 _sbPoint1;
	Vector3 _sbPoint2;

	void _OnSelectionChanged();

	const PropertyCollection& _GetProperties();

	Vector3 _GizmoMove(const Vector3& delta);
	float _GizmoRotate(const Vector3 &axis, float angle);
	void _GizmoSetSidePos(E2DBrushEdge edge, const Vector3& delta);
	void _UpdateGizmoTransform();

	void _SetHoverObject(WorldObject*);

	void _CloneSelection();

	bool _ViewportCanRaySelect(const Viewport&) const;
	bool _ViewportIsOrtho(const Viewport&) const;

public:
	ToolSelect(Editor& level) : Tool(level),
		_dragging(false),
		_placing(false),
		_shouldCopy(false),
		_hoverObject(),
		_hoverObjectIsSelected(false),
		_gridSnap(1.f),
		_snapToWorld(0),
		_gizmoIsLocal(0),
		_activeGizmo(nullptr)
	{}

	virtual ~ToolSelect() {}

	virtual void Initialise() override;

	virtual void Activate(UIContainer& properties, UIContainer& toolProperties) override;

	virtual void Cancel() override;

	virtual void MouseMove(MouseData&) override;
	virtual void MouseDown(MouseData&) override;
	virtual void MouseUp(MouseData&) override;
	virtual void KeySubmit() override;
	virtual void KeyDelete() override;

	virtual void Update(float deltaSeconds) override;
	virtual void Render(RenderQueue&) const override;

	void Select(WorldObject* object);
	void ClearSelection();
	void GetSelection(Buffer<WorldObject*>& outSelection) const;
	void SetSelection(const Buffer<WorldObject*>& selection);
};

