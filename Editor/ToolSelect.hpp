#pragma once
#include "Tool.hpp"
#include "Gizmo.hpp"
#include <Engine/Entity.hpp>
#include <ELCore/Buffer.hpp>
#include <ELCore/SharedPointer.hpp>

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

	Gizmo _gizmo;

	Vector3 _sbPoint1;
	Vector3 _sbPoint2;

	const PropertyCollection& _GetProperties();

	void _GizmoMove(const Vector3& delta);
	float _GizmoRotate(const Vector3 &axis, float angle);
	void _UpdateGizmoPos();

	void _SetHoverObject(Entity*);

	void _CloneSelection();

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
		_gizmo(
			Setter<const Vector3&>(this, &ToolSelect::_GizmoMove), 
			FunctionPointer<float, const Vector3 &, float>(this, &ToolSelect::_GizmoRotate)) 
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

