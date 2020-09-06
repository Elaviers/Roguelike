#pragma once
#include "Tool.hpp"
#include "Gizmo.hpp"
#include <Engine/Entity.hpp>
#include <ELCore/Buffer.hpp>
#include <ELCore/SharedPointer.hpp>

class Viewport;

class ToolSelect : public Tool
{
	bool _placing;
	bool _shouldCopy;

	EntityPointer _hoverObject;
	bool _hoverObjectIsSelected;

	Buffer<EntityPointer> _selectedObjects;

	float _origObjectX;
	float _origObjectY;

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

	const PropertyCollection& _GetProperties();

	Vector3 _GizmoMove(const Vector3& delta);
	float _GizmoRotate(const Vector3 &axis, float angle);
	void _GizmoSetSidePos(E2DBrushEdge edge, const Vector3& delta);
	void _UpdateGizmoTransform();

	void _SetHoverObject(Entity*);

	void _CloneSelection();

	bool _ViewportCanRaySelect(const Viewport&) const;

public:
	ToolSelect(Editor& level) : Tool(level),
		_placing(false),
		_shouldCopy(false),
		_hoverObject(),
		_hoverObjectIsSelected(false),
		_origObjectX(0.f),
		_origObjectY(0.f),
		_gridSnap(1.f),
		_snapToWorld(0),
		_gizmoIsLocal(0),
		_activeGizmo(nullptr)
	{}
	virtual ~ToolSelect() {}

	virtual void Initialise() override;

	virtual void Activate(UIContainer& properties, UIContainer& toolProperties) override;

	virtual void Cancel() override;

	virtual void MouseMove(const MouseData&) override;
	virtual void MouseDown(const MouseData&) override;
	virtual void MouseUp(const MouseData&) override;
	virtual void KeySubmit() override;
	virtual void KeyDelete() override;

	virtual void Render(RenderQueue&) const override;

	void Select(Entity* object);
	void ClearSelection();
};

