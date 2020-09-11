#pragma once
#include "EBrushEdge.hpp"
#include "GizmoComponent.hpp"
#include <ELCore/Concepts.hpp>
#include <ELCore/List.hpp>
#include <ELMaths/Ray.hpp>

/*
	Gizmo

	3d transformation thing
*/

class Gizmo
{
	List<GizmoComponent*> _components;

	Transform _transform;

	//Returns amount moved
	FunctionPointer<Vector3, const Vector3&> _fpMove;

	//The function takes an axis and an angle(degrees) and returns the angle consumed
	FunctionPointer<float, const Vector3&, float> _fpRotate;

	FunctionPointer<void, E2DBrushEdge, const Vector3&> _fpSetSide;

	Setter<const Vector3&> _fpScale; //unused
public:
	Gizmo() {}
	~Gizmo() 
	{
		for (GizmoComponent* c : _components)
			delete c;
	}

	const Transform& GetTransform() const { return _transform; }
	void SetTransform(const Transform& t) 
	{ 
		_transform = t; 

		for (GizmoComponent* c : _components)
			c->SetGizmoTransform(_transform);
	}

	void SetObjectTransform(const Transform& objt)
	{
		for (GizmoComponent* c : _components)
			c->SetObjectTransform(objt);
	}

	const FunctionPointer<Vector3, const Vector3&>& GetMoveFunction() const { return _fpMove; }
	const FunctionPointer<float, const Vector3&, float>& GetRotateFunction() const { return _fpRotate; }
	const Setter<const Vector3&>& GetScaleFunction() const { return _fpScale; }
	const FunctionPointer<void, E2DBrushEdge, const Vector3&>& GetSideFunction() const { return _fpSetSide; }

	void SetMoveFunction(const FunctionPointer<Vector3, const Vector3&>& move) { _fpMove = move; }
	void SetRotateFunction(const FunctionPointer<float, const Vector3&, float>& rotate) { _fpRotate = rotate; }
	void SetScaleFunction(const Setter<const Vector3&>& scale) { _fpScale = scale; }
	void SetSideFunction(const FunctionPointer<void, E2DBrushEdge, const Vector3&>& setSide) { _fpSetSide = setSide; }

	template <typename T>
	requires Concepts::DerivedFrom<T, GizmoComponent>
	void AddComponent(const T& component) { (*_components.Add(new T(component)))->_owner = this;  }
	void ClearComponents() 
	{ 
		for (GizmoComponent* c : _components)
			delete c;

		_components.Clear();
	}

	void Update(const MouseData &data, const Ray& mouseRay, float &maxT)
	{
		for (GizmoComponent* c : _components)
			c->Update(data, mouseRay, maxT);
	}

	bool MouseDown()
	{
		for (GizmoComponent* c : _components)
			if (c->MouseDown())
				return true;

		return false;
	}

	void MouseUp()
	{
		for (GizmoComponent* c : _components)
			c->MouseUp();
	}

	void Render(RenderQueue& q) const
	{
		for (const GizmoComponent* c : _components)
			c->Render(q);
	}
};
