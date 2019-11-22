#pragma once
#include "GizmoAxis.hpp"
#include "GizmoPlane.hpp"
#include "GizmoRing.hpp"
#include <Engine/Ray.hpp>

/*
	Gizmo

	3d transformation thing
*/

class Gizmo
{
	GizmoAxis _axisX;
	GizmoAxis _axisY;
	GizmoAxis _axisZ;
	GizmoPlane _planeXY;
	GizmoPlane _planeXZ;
	GizmoPlane _planeYZ;
	GizmoRing _ringX;
	GizmoRing _ringY;
	GizmoRing _ringZ;

	constexpr static int _componentCount = 9;
	GizmoComponent* const _all[_componentCount] = {
		&_axisX, &_axisY, &_axisZ, 
		&_planeXY, &_planeXZ, &_planeYZ,
		&_ringX, &_ringY, &_ringZ 
	};

public:
	Gizmo(const Setter<const Vector3&> &move, const FunctionPointer<float, const Vector3&, float> &rotate) :
		_axisX(Vector4(1, 0, 0), move),
		_axisY(Vector4(0, 1, 0), move),
		_axisZ(Vector4(0, 0, 1), move),
		_planeXY(Vector4(1, 1, 0), move),
		_planeXZ(Vector4(1, 0, 1), move),
		_planeYZ(Vector4(0, 1, 1), move),
		_ringX(Vector4(.5f, 0, 0), rotate),
		_ringY(Vector4(0, .5f, 0), rotate),
		_ringZ(Vector4(0, 0, .5f), rotate)
	{}

	~Gizmo() {}

	const Vector3& GetPosition() const { return _ringX.transform.GetPosition(); }

	void SetTransform(const Transform& t)
	{
		for (int i = 0; i < _componentCount; ++i)
			_all[i]->transform = t;

		_axisY.transform.Rotate(Quaternion(Vector3(1, 0, 0), 90.f));
		_axisX.transform.Rotate(Quaternion(Vector3(0, 1, 0), -90.f));

		_planeXZ.transform.Rotate(Quaternion(Vector3(1, 0, 0), 90.f));
		_planeYZ.transform.Rotate(Quaternion(Vector3(0, 1, 0), -90.f));

		_ringY.transform.Rotate(Quaternion(Vector3(1, 0, 0), 90.f));
		_ringX.transform.Rotate(Quaternion(Vector3(0, 1, 0), -90.f));

		_planeXY.transform.Move(Vector3(.5f, .5f, 0.f));
		_planeXZ.transform.Move(Vector3(.5f, 0.f, .5f));
		_planeYZ.transform.Move(Vector3(0.f, .5f, .5f));

		_planeXY.transform.SetScale(Vector3(.5f, .5f, .5f));
		_planeXZ.transform.SetScale(Vector3(.5f, .5f, .5f));
		_planeYZ.transform.SetScale(Vector3(.5f, .5f, .5f));
	}

	void Update(const MouseData &data, const Ray& mouseRay, float &maxT)
	{
		for (int i = 0; i < _componentCount; ++i)
		{
			_all[i]->Update(mouseRay, maxT);
		}
	}

	bool MouseDown()
	{
		for (int i = 0; i < _componentCount; ++i)
			if (_all[i]->MouseDown())
				return true;

		return false;
	}

	void MouseUp()
	{
		for (int i = 0; i < _componentCount; ++i)
			_all[i]->MouseUp();
	}

	void Draw() const
	{
		for (int i = 0; i < _componentCount; ++i)
			_all[i]->Draw();
	}
};
